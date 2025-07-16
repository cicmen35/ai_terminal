#ifndef TERMINAL_PANE_H
#define TERMINAL_PANE_H

#include <wx/panel.h>
#include <wx/textctrl.h>
#include "Autocomplete.h"

// Simple wrapper panel containing the terminal output window and input field.
// The constructor forwards back references to the created controls so that
// MainWindow can continue using them without changing all existing code.
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
