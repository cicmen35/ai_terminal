#ifndef AI_WORKER_H
#define AI_WORKER_H

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/string.h>

#include "AIHandler.h"

// Custom event fired when the background AI request completes
wxDECLARE_EVENT(wxEVT_AI_RESPONSE, wxThreadEvent);

//------------------------------------------------------------------------------
// AIWorkerThread
// Runs an OpenAI completion in a detached wxThread and posts the result back to
// the supplied wxEvtHandler (MainWindow) as a wxThreadEvent containing the
// assistant's reply in evt.GetString().
//------------------------------------------------------------------------------
class AIWorkerThread : public wxThread
{
public:
    AIWorkerThread(wxEvtHandler* handler,
                   AIHandler* aiHandler,
                   const wxString& question,
                   const wxString& context)
        : wxThread(wxTHREAD_DETACHED),
          m_handler(handler),
          m_aiHandler(aiHandler),
          m_question(question),
          m_context(context) {}

protected:
    virtual ExitCode Entry() override;

private:
    wxEvtHandler* m_handler;   // destination for the completion event (UI thread)
    AIHandler*    m_aiHandler; // not owned
    wxString      m_question;
    wxString      m_context;
};

#endif // AI_WORKER_H
