#include "mainWindow.h"
#include <wx/filename.h> // For wxFileName and path normalization

// IDs for the controls and the menu commands
enum
{
    ID_TERMINAL_INPUT = 1,
    ID_ASSISTANT_INPUT = 2
};

// Event table for MainWindow
wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_TEXT_ENTER(ID_TERMINAL_INPUT, MainWindow::OnCommandEntered)
    EVT_TEXT_ENTER(ID_ASSISTANT_INPUT, MainWindow::OnAskAssistant)
    EVT_IDLE(MainWindow::OnIdle)
    EVT_END_PROCESS(wxID_ANY, MainWindow::OnProcessTerminated)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 800))
{
    currentProcess = nullptr;
    currentPath = wxGetHomeDir(); // Set initial path to user's home directory
    wxSetWorkingDirectory(currentPath); // Change the application's CWD to user's home
    aiHandler = new AIHandler(); // Initialize AI Handler
    SetupUI();
    Centre();
}

// Destructor to clean up AIHandler
MainWindow::~MainWindow()
{
    delete aiHandler; // Clean up AI Handler
}

void MainWindow::SetupUI()
{
    // Main splitter window
    mainSplitter = new wxSplitterWindow(this, wxID_ANY);
    mainSplitter->SetMinimumPaneSize(200);

    // --- Terminal Panel (Left) ---
    terminalPanel = new wxPanel(mainSplitter);
    wxBoxSizer* terminalSizer = new wxBoxSizer(wxVERTICAL);
    terminalPanel->SetSizer(terminalSizer);

    terminalOutput = new wxTextCtrl(terminalPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    terminalOutput->SetBackgroundColour(wxColour(40, 40, 40));
    terminalOutput->SetForegroundColour(wxColour(248, 248, 242));
    terminalOutput->SetFont(wxFont(18, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    terminalSizer->Add(terminalOutput, 1, wxEXPAND | wxALL, 5);

    terminalInput = new wxTextCtrl(terminalPanel, ID_TERMINAL_INPUT, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    terminalInput->SetBackgroundColour(wxColour(40, 40, 40));
    terminalInput->SetForegroundColour(wxColour(248, 248, 242));
    terminalInput->SetFont(wxFont(18, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    terminalSizer->Add(terminalInput, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    // --- Assistant Panel (Right) ---
    assistantPanel = new wxPanel(mainSplitter);
    wxBoxSizer* assistantSizer = new wxBoxSizer(wxVERTICAL);
    assistantPanel->SetSizer(assistantSizer);

    assistantOutput = new wxTextCtrl(assistantPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    assistantOutput->SetBackgroundColour(wxColour(46, 52, 64));
    assistantOutput->SetForegroundColour(wxColour(236, 239, 244));
    assistantSizer->Add(assistantOutput, 1, wxEXPAND | wxALL, 5);

    assistantInput = new wxTextCtrl(assistantPanel, ID_ASSISTANT_INPUT, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    assistantInput->SetBackgroundColour(wxColour(46, 52, 64));
    assistantInput->SetForegroundColour(wxColour(236, 239, 244));
    assistantSizer->Add(assistantInput, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    // Split the window and set initial messages
    mainSplitter->SplitVertically(terminalPanel, assistantPanel, 800);
    terminalOutput->AppendText("Welcome to AI Terminal Emulator\n");
    assistantOutput->AppendText("AI Terminal Assistant\n");
}

void MainWindow::OnCommandEntered(wxCommandEvent& event)
{
    wxString command = terminalInput->GetValue();
    if (command.IsEmpty()) return;

    terminalOutput->AppendText("\n> " + command + "\n");
    terminalInput->Clear();

    ExecuteCommand(command);
}

void MainWindow::ExecuteCommand(const wxString& command)
{
    wxString mutableCommand = command; // Create a non-const copy
    wxString trimmedCommand = mutableCommand.Trim(); // Trim the copy

    // 1. Handle nano/vim (block them)
    if (trimmedCommand.StartsWith("nano") || trimmedCommand.StartsWith("vim")) { // Catches "nano", "vim", "nano file", "vim file"
        if (trimmedCommand == "nano" || trimmedCommand.StartsWith("nano ")) {
            terminalOutput->AppendText("nano is temporarily unavailable.\n");
        } else if (trimmedCommand == "vim" || trimmedCommand.StartsWith("vim ")) {
            terminalOutput->AppendText("vim is temporarily unavailable.\n");
        }
        return;
    }

    // 2. Handle clear
    if (trimmedCommand == "clear") {
        terminalOutput->Clear();
        return;
    }

    // 3. Handle cd commands
    if (trimmedCommand == "cd") { // 'cd' with no arguments
        currentPath = wxGetHomeDir();
        wxSetWorkingDirectory(currentPath); // Update internal CWD for wxProcess
        terminalOutput->AppendText(currentPath + "\n");
        return; 
    } else if (trimmedCommand.StartsWith("cd ")) { // 'cd <path>'
        wxString newPathArg = trimmedCommand.Mid(2).Trim(); // Get argument after "cd "

        if (newPathArg.IsEmpty()) { // Handles "cd " (cd followed by only spaces)
            currentPath = wxGetHomeDir();
        } else {
            wxFileName pathBuilder;
            if (wxIsAbsolutePath(newPathArg)) {
                pathBuilder.Assign(newPathArg);
            } else {
                // For relative paths, assign the relative path and then make it absolute
                pathBuilder.Assign(newPathArg);
                pathBuilder.MakeAbsolute(currentPath); 
            }
            // Normalize path (handles '..', redundant separators, etc.) and keep case.
            // Normalization should happen *after* path is potentially made absolute and combined.
            pathBuilder.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE);

            if (wxDirExists(pathBuilder.GetFullPath())) {
                currentPath = pathBuilder.GetFullPath();
            } else {
                terminalOutput->AppendText("cd: no such file or directory: " + newPathArg + "\n");
                return; // Return if path is invalid
            }
        }
        wxSetWorkingDirectory(currentPath); // Update internal CWD for wxProcess
        terminalOutput->AppendText(currentPath + "\n");
        return; // Return after successful cd
    }

    // 4. If none of the above, execute as a general command
    currentProcess = new wxProcess(this);
    currentProcess->Redirect();

    // Use the original 'command' for wxExecute for fidelity, not trimmedCommand.
    long pid = wxExecute(command, wxEXEC_ASYNC, currentProcess);
    if (!pid) {
        terminalOutput->AppendText("Error: Command could not be executed.\n");
        delete currentProcess;
        currentProcess = nullptr;
    }
}

void MainWindow::OnAskAssistant(wxCommandEvent& event)
{
    wxString question = assistantInput->GetValue();
    if (question.IsEmpty()) return;

    assistantInput->Clear();
    assistantOutput->AppendText("You: " + question + "\n");

    // Get the last 1000 characters from the terminal output as context
    wxString terminalContent = terminalOutput->GetValue();
    wxString context = terminalContent.Right(1000); // Get last 1000 chars

    // Get response from AI Handler, now with context
    wxString aiResponse = aiHandler->GetAIResponse(question, context);
    assistantOutput->AppendText("Assistant: " + aiResponse + "\n");
    assistantOutput->ShowPosition(assistantOutput->GetLastPosition()); // Scroll to show the latest response
}

void MainWindow::OnIdle(wxIdleEvent& event)
{
    if (!currentProcess) return;

    wxInputStream* in = currentProcess->GetInputStream();
    if (in && in->IsOk() && in->CanRead())
    {
        wxTextInputStream text_in(*in);
        while (in->CanRead())
        {
            terminalOutput->AppendText(text_in.ReadLine() + "\n");
        }
    }

    wxInputStream* err = currentProcess->GetErrorStream();
    if (err && err->IsOk() && err->CanRead())
    {
        wxTextInputStream text_err(*err);
        while (err->CanRead())
        {
            terminalOutput->AppendText(text_err.ReadLine() + "\n");
        }
    }
}

void MainWindow::OnProcessTerminated(wxProcessEvent& event)
{
    wxInputStream* in = currentProcess->GetInputStream();
    if (in && in->IsOk() && in->CanRead())
    {
        wxTextInputStream text_in(*in);
        while (in->CanRead())
        {
            terminalOutput->AppendText(text_in.ReadLine() + "\n");
        }
    }

    if (event.GetExitCode() != 0)
    {
        terminalOutput->AppendText(wxString::Format("Process %ld terminated with error code %d.\n", (long)event.GetPid(), event.GetExitCode()));
    }
    delete currentProcess;
    currentProcess = nullptr;
}
