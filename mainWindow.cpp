#include "mainWindow.h"
#include <QProcess>
#include <QDebug>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Set window properties
    setWindowTitle("AI Terminal Emulator");
    resize(1200, 800);
    
    // Create central widget and main layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create main splitter to divide terminal and assistant panels
    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(mainSplitter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Setup terminal and assistant panels
    setupTerminalPanel();
    setupAssistantPanel();
    
    // Set initial splitter sizes (70% terminal, 30% assistant)
    QList<int> sizes;
    sizes << (width() * 0.7) << (width() * 0.3);
    mainSplitter->setSizes(sizes);
}

MainWindow::~MainWindow()
{
    // Cleanup will be handled by Qt's parent-child relationship
}

void MainWindow::setupTerminalPanel()
{
    // Create terminal panel container
    terminalPanel = new QWidget(mainSplitter);
    QVBoxLayout *terminalLayout = new QVBoxLayout(terminalPanel);
    terminalLayout->setContentsMargins(5, 5, 5, 5);
    
    // Create terminal output display
    terminalOutput = new QTextEdit(terminalPanel);
    terminalOutput->setReadOnly(true);
    terminalOutput->setStyleSheet("QTextEdit { background-color: #282828; color: #F8F8F2; font-family: 'Courier New', monospace; }");
    terminalLayout->addWidget(terminalOutput);
    
    // Create terminal input field
    terminalInput = new QLineEdit(terminalPanel);
    terminalInput->setStyleSheet("QLineEdit { background-color: #282828; color: #F8F8F2; font-family: 'Courier New', monospace; }");
    terminalInput->setPlaceholderText("Enter command...");
    terminalLayout->addWidget(terminalInput);
    
    // Connect signals
    connect(terminalInput, &QLineEdit::returnPressed, this, &MainWindow::onCommandEntered);
    
    // Add welcome message
    terminalOutput->append("Welcome to AI Terminal Emulator");
    terminalOutput->append("Type commands in the input field below.");
    terminalOutput->append("-----------------------------------");
}

void MainWindow::setupAssistantPanel()
{
    // Create assistant panel container
    assistantPanel = new QWidget(mainSplitter);
    QVBoxLayout *assistantLayout = new QVBoxLayout(assistantPanel);
    assistantLayout->setContentsMargins(5, 5, 5, 5);
    
    // Create assistant output display
    assistantOutput = new QTextEdit(assistantPanel);
    assistantOutput->setReadOnly(true);
    assistantOutput->setStyleSheet("QTextEdit { background-color: #2E3440; color: #ECEFF4; font-family: 'Segoe UI', sans-serif; }");
    assistantLayout->addWidget(assistantOutput);
    
    // Create assistant input field
    assistantInput = new QLineEdit(assistantPanel);
    assistantInput->setStyleSheet("QLineEdit { background-color: #2E3440; color: #ECEFF4; font-family: 'Segoe UI', sans-serif; }");
    assistantInput->setPlaceholderText("Ask AI assistant...");
    assistantLayout->addWidget(assistantInput);
    
    // Connect signals
    connect(assistantInput, &QLineEdit::returnPressed, this, &MainWindow::onAskAssistant);
    
    // Add welcome message
    assistantOutput->append("AI Terminal Assistant");
    assistantOutput->append("Ask questions about commands or get help with terminal tasks.");
    assistantOutput->append("-----------------------------------");
}

void MainWindow::onCommandEntered()
{
    QString command = terminalInput->text();
    terminalInput->clear();
    
    // Display the command in the terminal output
    terminalOutput->append("> " + command);
    
    // Execute the command (to be implemented)
    executeCommand(command);
}

void MainWindow::onAskAssistant()
{
    QString question = assistantInput->text();
    assistantInput->clear();
    
    // Display the question in the assistant output
    assistantOutput->append("You: " + question);
    
    // This is a placeholder - we'll implement the actual AI integration later
    assistantOutput->append("Assistant: I'll help you with that in a future implementation!");
}

void MainWindow::executeCommand(const QString &command)
{
    // This is a placeholder - we'll implement actual command execution later
    terminalOutput->append("Command execution will be implemented in the next step.");
}
