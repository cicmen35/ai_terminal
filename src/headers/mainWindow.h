#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/process.h>
#include <wx/txtstrm.h>
#include <memory>
#include "ai_handler.h"
#include "autocomplete.h" 
#include "ai_worker.h"

class MainWindow : public wxFrame
{
public:
    MainWindow(const wxString& title);
    ~MainWindow(); // Declare the destructor

private:
    // Event handlers
    void OnCommandEntered(wxCommandEvent& event);
    void OnAskAssistant(wxCommandEvent& event);
    void OnProcessTerminated(wxProcessEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnAIResponse(wxThreadEvent& event);

    // Helper methods
    void SetupUI();
    void ExecuteCommand(const wxString& command);

    // UI Components
    wxSplitterWindow* mainSplitter;
    wxPanel* terminalPanel;
    wxTextCtrl* terminalOutput;
    AutocompleteInput* terminalInput;
    wxPanel* assistantPanel;
    wxTextCtrl* assistantOutput;
    wxTextCtrl* assistantInput;

    // Process handling
    std::unique_ptr<wxProcess> currentProcess;
    wxString currentPath;

    // AI Handler
    std::unique_ptr<AIHandler> aiHandler; 

    // Event table declaration
    wxDECLARE_EVENT_TABLE();
};

#endif // MAINWINDOW_H
