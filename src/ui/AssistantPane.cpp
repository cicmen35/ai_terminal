#include "AssistantPane.h"
#include <wx/sizer.h>

AssistantPane::AssistantPane(wxWindow* parent,
                             int inputId,
                             wxTextCtrl*& outOutputCtrl,
                             wxTextCtrl*& outInputCtrl)
    : wxPanel(parent, wxID_ANY)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    m_output = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    m_output->SetBackgroundColour(wxColour(46, 52, 64));
    m_output->SetForegroundColour(wxColour(236, 239, 244));
    sizer->Add(m_output, 1, wxEXPAND | wxALL, 5);

    m_input = new wxTextCtrl(this, inputId, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_input->SetBackgroundColour(wxColour(46, 52, 64));
    m_input->SetForegroundColour(wxColour(236, 239, 244));
    sizer->Add(m_input, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    outOutputCtrl = m_output;
    outInputCtrl  = m_input;
}
