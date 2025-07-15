#include "TerminalPane.h"
#include <wx/sizer.h>

TerminalPane::TerminalPane(wxWindow* parent,
                           int inputId,
                           wxTextCtrl*& outOutputCtrl,
                           AutocompleteInput*& outInputCtrl)
    : wxPanel(parent, wxID_ANY)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    m_output = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    m_output->SetBackgroundColour(wxColour(40, 40, 40));
    m_output->SetForegroundColour(wxColour(248, 248, 242));
    m_output->SetFont(wxFont(18, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    sizer->Add(m_output, 1, wxEXPAND | wxALL, 5);

    m_input = new AutocompleteInput(this, inputId, m_output, "", wxDefaultPosition, wxDefaultSize,
                                    wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
    m_input->SetBackgroundColour(wxColour(40, 40, 40));
    m_input->SetForegroundColour(wxColour(248, 248, 242));
    m_input->SetFont(wxFont(18, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    sizer->Add(m_input, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    outOutputCtrl = m_output;
    outInputCtrl  = m_input;
}
