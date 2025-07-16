#ifndef AUTOCOMPLETE_INPUT_H
#define AUTOCOMPLETE_INPUT_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <vector>
#include <string>

class AutocompleteInput : public wxTextCtrl {
public:
    AutocompleteInput(wxWindow* parent, 
                 wxWindowID id,
                 wxTextCtrl* outputCtrl,
                 const wxString& value = wxEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0);

    void SetCurrentPath(const wxString& path) { m_currentPath = path; }
    wxString GetCurrentPath() const { return m_currentPath; }

    // Command history operations
    void AddToHistory(const wxString& command);

private:
    void OnKeyDown(wxKeyEvent& event);
    void HandleTabCompletion();
    std::vector<wxString> GetCompletionCandidates(const wxString& partial) const;
    wxString GetCurrentWord() const;

    // History navigation
    std::vector<wxString> m_history;
    int m_historyPos = -1;

    wxString m_currentPath;
    wxTextCtrl* m_outputCtrl;
    
    DECLARE_EVENT_TABLE()
};

#endif // AUTOCOMPLETE_INPUT_H
