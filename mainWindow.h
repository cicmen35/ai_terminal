#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/process.h>
#include <wx/txtstrm.h>

class MainWindow : public wxFrame
{
public:
    MainWindow(const wxString& title);

private:
    // Event handlers
    void OnCommandEntered(wxCommandEvent& event);
    void OnAskAssistant(wxCommandEvent& event);
    void OnProcessTerminated(wxProcessEvent& event);
    void OnIdle(wxIdleEvent& event);

    // Helper methods
    void SetupUI();
    void ExecuteCommand(const wxString& command);

    // UI Components
    wxSplitterWindow* mainSplitter;
    wxPanel* terminalPanel;
    wxTextCtrl* terminalOutput;
    wxTextCtrl* terminalInput;
    wxPanel* assistantPanel;
    wxTextCtrl* assistantOutput;
    wxTextCtrl* assistantInput;

    // Process handling
    wxProcess* currentProcess;
    wxString currentPath;

    // Event table declaration
    wxDECLARE_EVENT_TABLE();
};

#endif // MAINWINDOW_H
