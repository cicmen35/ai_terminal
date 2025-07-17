#ifndef TERMINAL_PANE_H
#define TERMINAL_PANE_H

#include <wx/panel.h>
#include <wx/textctrl.h>
#include "Autocomplete.h"

class TerminalPane : public wxPanel
{
public:
    // outOutputCtrl / outInputCtrl are filled with the created widgets so the
    // caller can keep using them.
    TerminalPane(wxWindow* parent,
                 int inputId,
                 wxTextCtrl*& outOutputCtrl,
                 AutocompleteInput*& outInputCtrl);

private:
    wxTextCtrl* m_output;
    AutocompleteInput* m_input;
};

#endif // TERMINAL_PANE_H
