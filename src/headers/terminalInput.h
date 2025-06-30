#ifndef TERMINAL_INPUT_H
#define TERMINAL_INPUT_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <vector>
#include <string>

class TerminalInput : public wxTextCtrl {
public:
    TerminalInput(wxWindow* parent, 
                 wxWindowID id,
                 const wxString& value = wxEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0);

    void SetCurrentPath(const wxString& path) { m_currentPath = path; }
    wxString GetCurrentPath() const { return m_currentPath; }

private:
    void OnKeyDown(wxKeyEvent& event);
    void HandleTabCompletion();
    std::vector<wxString> GetCompletionCandidates(const wxString& partial) const;
    wxString GetCurrentWord() const;

    wxString m_currentPath;
    
    DECLARE_EVENT_TABLE()
};

#endif // TERMINAL_INPUT_H
