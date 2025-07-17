#include "wx/wx.h"
#include "ui/headers/mainWindow.h"

// Define a new application type
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MainWindow *frame = new MainWindow("AI Terminal");
    frame->Show(true);
    return true;
}
