#ifndef AI_STREAM_WORKER_H
#define AI_STREAM_WORKER_H

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/string.h>
#include "AIHandler.h"

// Event carries chunks of the streamed response.
// evt.GetString() -> next token
// evt.GetInt() == 1 when streaming finished
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
    void HandleBuffer(const std::string& bufferPart);

    wxEvtHandler* m_handler;
    AIHandler* m_aiHandler; // not owned
    std::string m_requestJson;
    std::string m_accumBuffer; // buffer for partial SSE lines
};

#endif // AI_STREAM_WORKER_H
