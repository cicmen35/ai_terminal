#include "wx/wx.h"
#include "mainWindow.h"

// Define a new application type
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// Implement the application's entry point
wxIMPLEMENT_APP(MyApp);

// This is called on application startup
bool MyApp::OnInit()
{
    // Create the main application window
    MainWindow *frame = new MainWindow("AI Terminal Emulator");
    frame->Show(true);
    return true;
}
