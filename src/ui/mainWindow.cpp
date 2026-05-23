#include "mainWindow.h"
#include <array>
#include <wx/filename.h> // For wxFileName and path normalization
#include <wx/utils.h>   // For wxProcess::Kill
#include <memory>

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
    currentProcess.reset();
    currentPath = wxGetHomeDir(); // Set initial path to user's home directory
    wxSetWorkingDirectory(currentPath); // Change the application's CWD to user's home
    aiHandler = std::make_unique<AIHandler>(); // Initialize AI Handler
    SetupUI();
    Bind(wxEVT_AI_STREAM_CHUNK, &MainWindow::OnAIStreamChunk, this);
    Centre();
}

// Destructor to clean up AIHandler
MainWindow::~MainWindow()
{
    if (currentProcess) {
        const long pid = currentProcess->GetPid();
        currentProcess->Detach();
        if (pid > 0 && wxProcess::Exists(static_cast<int>(pid))) {
            wxProcess::Kill(static_cast<int>(pid), wxSIGTERM);
        }
        currentProcess.reset();
    }
}

void MainWindow::SetupUI()
{
    // Main splitter window
    mainSplitter = new wxSplitterWindow(this, wxID_ANY);
    mainSplitter->SetMinimumPaneSize(200);

    // Create our encapsulated panes. They hand back the pointers to
    // the controls we still use throughout MainWindow.
    terminalPane  = new TerminalPane(mainSplitter, ID_TERMINAL_INPUT, terminalOutput, terminalInput);
    assistantPane = new AssistantPane(mainSplitter, ID_ASSISTANT_INPUT, assistantOutput, assistantInput);
    terminalInput->SetCurrentPath(currentPath);

    // Layout
    mainSplitter->SplitVertically(terminalPane, assistantPane, 800);

    terminalOutput->AppendText("Welcome to AI Terminal Emulator\n");
    assistantOutput->AppendText("AI Terminal Assistant\n");
}

void MainWindow::OnCommandEntered(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString command = terminalInput->GetValue();
    if (command.IsEmpty()) return;

    terminalOutput->AppendText("\n> " + command + "\n");
    terminalInput->AddToHistory(command);
    terminalInput->Clear();

    ExecuteCommand(command);
}

void MainWindow::ExecuteCommand(const wxString& command)
{
    wxString trimmedCommand = command;
    trimmedCommand.Trim(true).Trim(false);

    if (HandleBuiltinCommand(trimmedCommand)) {
        return;
    }

    if (HasRunningProcess()) {
        terminalOutput->AppendText("A command is already running. Wait for it to finish before starting another.\n");
        return;
    }

    currentProcess = std::make_unique<wxProcess>(this);
    currentProcess->Redirect();

    long pid = wxExecute(command, wxEXEC_ASYNC, currentProcess.get());
    if (!pid) {
        terminalOutput->AppendText("Error: Command could not be executed.\n");
        currentProcess.reset();
    }
}

void MainWindow::OnAskAssistant(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString question = assistantInput->GetValue();
    if (question.IsEmpty()) return;

    if (assistantResponseInProgress) {
        assistantOutput->AppendText("Assistant: Wait for the current response to finish before asking another question.\n");
        assistantOutput->ShowPosition(assistantOutput->GetLastPosition());
        return;
    }

    assistantInput->Clear();
    assistantOutput->AppendText("You: " + question + "\n");

    if (aiHandler->GetAPIKey().empty()) {
        assistantOutput->AppendText("Assistant: OpenAI API key not configured. Copy .env next to the app binary or set OPENAI_API_TOKEN.\n");
        assistantOutput->ShowPosition(assistantOutput->GetLastPosition());
        return;
    }

    // Get the last 1000 characters from the terminal output as context
    wxString terminalContent = terminalOutput->GetValue();
    wxString context = terminalContent.Right(1000); // Get last 1000 chars

    AIStreamWorkerThread* worker = new AIStreamWorkerThread(this, aiHandler.get(), question, context);
    assistantOutput->AppendText("Assistant: ");
    assistantResponseInProgress = true;
    assistantResponseHasContent = false;

    if (worker->Run() != wxTHREAD_NO_ERROR) {
        assistantOutput->AppendText("failed to start background task.");
        FinishAssistantResponse();
        delete worker;
    }
}

void MainWindow::OnAIStreamChunk(wxThreadEvent& event)
{
    if (event.GetInt() == AI_STREAM_CHUNK) {
        assistantResponseHasContent = true;
        assistantOutput->AppendText(event.GetString());
    } else if (event.GetInt() == AI_STREAM_COMPLETE) {
        FinishAssistantResponse();
    } else if (event.GetInt() == AI_STREAM_ERROR) {
        if (assistantResponseHasContent) {
            assistantOutput->AppendText("\n");
        }
        assistantOutput->AppendText(event.GetString());
        FinishAssistantResponse();
    }
    assistantOutput->ShowPosition(assistantOutput->GetLastPosition());
}

void MainWindow::OnIdle(wxIdleEvent& event)
{
    wxUnusedVar(event);
    DrainProcessStreams();
}

void MainWindow::OnProcessTerminated(wxProcessEvent& event)
{
    if (!currentProcess) {
        return;
    }

    DrainProcessStreams();

    if (event.GetExitCode() != 0) {
        terminalOutput->AppendText(wxString::Format("Process %ld terminated with error code %d.\n", (long)event.GetPid(), event.GetExitCode()));
    }

    currentProcess->Detach();
    currentProcess.reset();
}

bool MainWindow::HandleBuiltinCommand(const wxString& trimmedCommand)
{
    if (trimmedCommand == "nano" || trimmedCommand.StartsWith("nano ")) {
        terminalOutput->AppendText("nano is temporarily unavailable.\n");
        return true;
    }

    if (trimmedCommand == "vim" || trimmedCommand.StartsWith("vim ")) {
        terminalOutput->AppendText("vim is temporarily unavailable.\n");
        return true;
    }

    if (trimmedCommand == "clear") {
        terminalOutput->Clear();
        return true;
    }

    if (trimmedCommand == "cd") {
        return ChangeDirectory(wxEmptyString);
    }

    if (trimmedCommand.StartsWith("cd ")) {
        wxString newPathArg = trimmedCommand.Mid(2);
        newPathArg.Trim(true).Trim(false);
        return ChangeDirectory(newPathArg);
    }

    return false;
}

bool MainWindow::ChangeDirectory(const wxString& pathArg)
{
    if (pathArg.IsEmpty()) {
        currentPath = wxGetHomeDir();
    } else {
        wxFileName pathBuilder;
        if (wxIsAbsolutePath(pathArg)) {
            pathBuilder.Assign(pathArg);
        } else {
            pathBuilder.Assign(currentPath, pathArg);
        }

        pathBuilder.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE);

        if (!wxDirExists(pathBuilder.GetFullPath())) {
            terminalOutput->AppendText("cd: no such file or directory: " + pathArg + "\n");
            return true;
        }

        currentPath = pathBuilder.GetFullPath();
    }

    wxSetWorkingDirectory(currentPath);
    terminalInput->SetCurrentPath(currentPath);
    terminalOutput->AppendText(currentPath + "\n");
    return true;
}

bool MainWindow::HasRunningProcess() const
{
    return currentProcess != nullptr;
}

void MainWindow::DrainProcessStreams()
{
    if (!currentProcess) {
        return;
    }

    DrainProcessStream(currentProcess->GetInputStream());
    DrainProcessStream(currentProcess->GetErrorStream());
}

void MainWindow::DrainProcessStream(wxInputStream* stream)
{
    if (!stream || !stream->IsOk() || !stream->CanRead()) {
        return;
    }

    std::array<char, 4096> buffer{};
    while (stream->CanRead()) {
        stream->Read(buffer.data(), buffer.size());
        const size_t bytesRead = stream->LastRead();
        if (bytesRead == 0) {
            break;
        }

        terminalOutput->AppendText(wxString::From8BitData(buffer.data(), bytesRead));
    }
}

void MainWindow::FinishAssistantResponse(bool appendTrailingNewline)
{
    if (!assistantResponseInProgress) {
        return;
    }

    if (appendTrailingNewline) {
        assistantOutput->AppendText("\n");
    }
    assistantResponseInProgress = false;
    assistantResponseHasContent = false;
}
