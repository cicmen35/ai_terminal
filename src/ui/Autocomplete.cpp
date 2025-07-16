#include "Autocomplete.h"
#include <wx/dir.h>
#include <algorithm>

BEGIN_EVENT_TABLE(AutocompleteInput, wxTextCtrl)
    EVT_KEY_DOWN(AutocompleteInput::OnKeyDown)
END_EVENT_TABLE()

AutocompleteInput::AutocompleteInput(wxWindow* parent,
                           wxWindowID id,
                            wxTextCtrl* outputCtrl,
                           const wxString& value,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style)
    : wxTextCtrl(parent, id, value, pos, size, style),
      m_outputCtrl(outputCtrl)
{
        m_currentPath = wxGetHomeDir();
}

void AutocompleteInput::OnKeyDown(wxKeyEvent& event)
{
    int key = event.GetKeyCode();
    if (key == WXK_UP && !event.HasAnyModifiers()) {
        if (!m_history.empty()) {
            if (m_historyPos == -1) m_historyPos = static_cast<int>(m_history.size()) - 1;
            else if (m_historyPos > 0) --m_historyPos;
            SetValue(m_history[m_historyPos]);
            SetInsertionPointEnd();
        }
        return;
    } else if (key == WXK_DOWN && !event.HasAnyModifiers()) {
        if (!m_history.empty() && m_historyPos != -1) {
            if (m_historyPos < static_cast<int>(m_history.size()) - 1) {
                ++m_historyPos;
                SetValue(m_history[m_historyPos]);
            } else {
                m_historyPos = -1;
                Clear();
            }
            SetInsertionPointEnd();
        }
        return;
    }
    if (key == WXK_TAB && !event.HasAnyModifiers()) {
        HandleTabCompletion();
        return; 
    }
    // Reset history position on any other key press (except navigation)
    m_historyPos = -1;
    event.Skip(); // Skip for remaining keys
}

void AutocompleteInput::AddToHistory(const wxString& command)
{
    wxString trimmed = command;
    trimmed.Trim(true);
    trimmed.Trim(false);
    if (trimmed.IsEmpty()) return;
    if (m_history.empty() || m_history.back() != trimmed)
        m_history.push_back(trimmed);
    m_historyPos = -1;
}

wxString AutocompleteInput::GetCurrentWord() const
{
    long insertionPoint = GetInsertionPoint();
    wxString text = GetValue();
    
    // Find the start of the current word
    long start = insertionPoint;
    while (start > 0 && text[start - 1] != ' ') {
        --start;
    }
    
    // Extract the partial word
    return text.Mid(start, insertionPoint - start);
}

std::vector<wxString> AutocompleteInput::GetCompletionCandidates(const wxString& partial) const
{
    std::vector<wxString> candidates;
    wxString searchPath = m_currentPath;
    wxString prefix;

    // Get the full command text and check if it starts with 'cd '
    wxString fullCommand = GetValue();
    bool isCdCommand = fullCommand.StartsWith("cd ");

    // Handle relative paths
    size_t lastSlash = partial.find_last_of("/");
    if (lastSlash != wxString::npos) {
        wxFileName fn(m_currentPath, "");
        fn.AppendDir(partial.Left(lastSlash));
        searchPath = fn.GetPath();
        prefix = partial.Left(lastSlash + 1);
    }

    wxDir dir(searchPath);
    if (!dir.IsOpened()) {
        return candidates;
    }

    wxString filename;
    wxString filespec;
    
    // If it's just 'cd ' followed by a single character, show all options
    if (isCdCommand && partial.Length() == 1) {
        filespec = partial + "*";
    } else {
        filespec = partial.Mid(lastSlash + 1) + "*";
    }
    
    bool cont = dir.GetFirst(&filename, filespec, wxDIR_FILES | wxDIR_DIRS);
    while (cont) {
        // For cd command, only include directories and don't add trailing slash
        if (isCdCommand) {
            if (wxDirExists(searchPath + "/" + filename)) {
                candidates.push_back(prefix + filename);
            }
        } else {
            // For other commands, include both files and directories
            if (wxDirExists(searchPath + "/" + filename)) {
                filename += "/";
            }
            candidates.push_back(prefix + filename);
        }
        cont = dir.GetNext(&filename);
    }

    // Sort candidates for consistent presentation
    std::sort(candidates.begin(), candidates.end());
    
    return candidates;
}

void AutocompleteInput::HandleTabCompletion()
{
    wxString partial = GetCurrentWord();
    if (partial.IsEmpty()) {
        return;
    }

    std::vector<wxString> candidates = GetCompletionCandidates(partial);
    
    if (candidates.empty()) {
        return;
    } else if (candidates.size() == 1) {
        // Single match - auto-complete
        wxString text = GetValue();
        long insertionPoint = GetInsertionPoint();
        long start = insertionPoint;
        
        // Find start of the word to replace
        while (start > 0 && text[start - 1] != ' ') {
            --start;
        }
        
        // Replace the partial word with the complete one
        Replace(start, insertionPoint, candidates[0]);
        SetInsertionPointEnd();
    } else {
        // Multiple matches - show options
        wxString commonPrefix = candidates[0];
        
        // find the common prefix among all candidates
        for (const auto& candidate : candidates) {
            size_t len = std::min(commonPrefix.length(), candidate.length());
            for (size_t i = 0; i < len; ++i) {
                if (commonPrefix[i] != candidate[i]) {
                    commonPrefix = commonPrefix.Left(i);
                    break;
                }
            }
        }
        
        // Show all candidates
        if (m_outputCtrl && !candidates.empty()) {
            wxString output = "\n";
            for (size_t i = 0; i < candidates.size(); ++i) {
                output += candidates[i];
                if (i < candidates.size() - 1) output += "  ";
            }
            output += "\n> " + GetValue();
            m_outputCtrl->AppendText(output);
        }
        
        // If we have a longer common prefix than what's typed, complete to that
        if (commonPrefix.length() > partial.length()) {
            wxString text = GetValue();
            long insertionPoint = GetInsertionPoint();
            long start = insertionPoint;
            
            while (start > 0 && text[start - 1] != ' ') {
                --start;
            }
            
            Replace(start, insertionPoint, commonPrefix);
            SetInsertionPointEnd();
        }
    }
}
