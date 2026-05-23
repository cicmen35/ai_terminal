#ifndef AI_STREAM_WORKER_H
#define AI_STREAM_WORKER_H

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/string.h>
#include "AIHandler.h"

enum AIStreamEventKind
{
    AI_STREAM_CHUNK = 0,
    AI_STREAM_COMPLETE = 1,
    AI_STREAM_ERROR = 2
};

// Event carries chunks of the streamed response or a terminal status message.
// evt.GetString() -> token text or error message depending on evt.GetInt().
wxDECLARE_EVENT(wxEVT_AI_STREAM_CHUNK, wxThreadEvent);

class AIStreamWorkerThread : public wxThread {
public:
    AIStreamWorkerThread(wxEvtHandler* handler,
                         AIHandler* aiHandler,
                         const wxString& question,
                         const wxString& context);

protected:
    virtual ExitCode Entry() override;

private:
    static size_t CurlWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
    void PostEvent(int kind, const wxString& text = wxEmptyString);
    void PostError(const wxString& text);
    void PostComplete();
    void HandleBuffer(const std::string& bufferPart);
    wxString BuildAPIErrorMessage(long responseCode) const;

    wxEvtHandler* m_handler;
    AIHandler* m_aiHandler; // not owned
    std::string m_requestJson;
    std::string m_accumBuffer; // buffer for partial SSE lines
    std::string m_responseBody;
    bool m_sawStreamEvent = false;
    bool m_terminalEventPosted = false;
};

#endif // AI_STREAM_WORKER_H
