#ifndef ASSISTANT_PANE_H
#define ASSISTANT_PANE_H

#include <wx/panel.h>
#include <wx/textctrl.h>

// Panel hosting the assistant chat history and input field.
class AssistantPane : public wxPanel
{
public:
    AssistantPane(wxWindow* parent,
                  int inputId,
                  wxTextCtrl*& outOutputCtrl,
                  wxTextCtrl*& outInputCtrl);

private:
    wxTextCtrl* m_output;
    wxTextCtrl* m_input;
};

#endif // ASSISTANT_PANE_H
