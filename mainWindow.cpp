#include "mainWindow.h"

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
    currentPath = wxGetCwd();
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
    // condition for user trying to open a file via nano/vim
    if (command.StartsWith("nano ") || command.StartsWith("vim "))
    {
        terminalOutput->AppendText(command.StartsWith("nano ") ? "nano is temporarily unavailable.\n" : "vim is temporarily unavailable.\n");
        return;
    }

    // Handle 'clear' command
    if (command == "clear")
    {
        terminalOutput->Clear();
        return;
    }

    // Handle 'cd' command separately
    if (command.StartsWith("cd "))
    {
        wxString newDir = command.Mid(3);
        if (!wxSetWorkingDirectory(newDir))
        {
            terminalOutput->AppendText("cd: No such file or directory: " + newDir + "\n");
        }
        currentPath = wxGetCwd();
        terminalOutput->AppendText("Current directory: " + currentPath + "\n");
        return;
    }

    currentProcess = new wxProcess(this);
    currentProcess->Redirect();

    long pid = wxExecute(command, wxEXEC_ASYNC, currentProcess);
    if (!pid)
    {
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

    // Get response from AI Handler
    wxString aiResponse = aiHandler->GetAIResponse(question);
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
