#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Terminal input handling
    void onCommandEntered();
    
    // Future AI assistant integration
    void onAskAssistant();

private:
    // Main layout components
    QWidget *centralWidget;
    QSplitter *mainSplitter;
    
    // Terminal components
    QWidget *terminalPanel;
    QTextEdit *terminalOutput;
    QLineEdit *terminalInput;
    
    // AI Assistant components (to be expanded)
    QWidget *assistantPanel;
    QTextEdit *assistantOutput;
    QLineEdit *assistantInput;
    
    // Helper methods
    void setupTerminalPanel();
    void setupAssistantPanel();
    void executeCommand(const QString &command);
};

#endif // MAINWINDOW_H
