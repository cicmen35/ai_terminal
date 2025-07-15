#include "ai_worker.h"

wxDEFINE_EVENT(wxEVT_AI_RESPONSE, wxThreadEvent);

wxThread::ExitCode AIWorkerThread::Entry()
{
    // Run the potentially long-running network call
    wxString reply = m_aiHandler->GetAIResponse(m_question, m_context);

    // Post back to the UI thread
    wxThreadEvent evt(wxEVT_AI_RESPONSE);
    evt.SetString(reply);
    wxQueueEvent(m_handler, evt.Clone());
    return (wxThread::ExitCode)0;
}
