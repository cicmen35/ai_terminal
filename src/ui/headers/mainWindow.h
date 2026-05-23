#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wx/wx.h>
#include <wx/stream.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/process.h>
#include <memory>
#include "AIHandler.h"
#include "Autocomplete.h"
#include "TerminalPane.h"
#include "AssistantPane.h"
#include "AIStreamWorker.h"

class MainWindow : public wxFrame
{
public:
    MainWindow(const wxString& title);
    ~MainWindow(); 

private:
    // Event handlers
    void OnCommandEntered(wxCommandEvent& event);
    void OnAskAssistant(wxCommandEvent& event);
    void OnProcessTerminated(wxProcessEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnAIStreamChunk(wxThreadEvent& event);

    // Helper methods
    void SetupUI();
    void ExecuteCommand(const wxString& command);
    bool HandleBuiltinCommand(const wxString& trimmedCommand);
    bool ChangeDirectory(const wxString& pathArg);
    bool HasRunningProcess() const;
    void DrainProcessStreams();
    void DrainProcessStream(wxInputStream* stream);
    void FinishAssistantResponse(bool appendTrailingNewline = true);

    // UI Components
    wxSplitterWindow* mainSplitter;
    TerminalPane* terminalPane;
    wxTextCtrl* terminalOutput;
    AutocompleteInput* terminalInput;
    AssistantPane* assistantPane;
    wxTextCtrl* assistantOutput;
    wxTextCtrl* assistantInput;

    // Process handling
    std::unique_ptr<wxProcess> currentProcess;
    wxString currentPath;

    // AI Handler
    std::unique_ptr<AIHandler> aiHandler;
    bool assistantResponseInProgress = false;
    bool assistantResponseHasContent = false;

    // Event table declaration
    wxDECLARE_EVENT_TABLE();
};

#endif // MAINWINDOW_H
