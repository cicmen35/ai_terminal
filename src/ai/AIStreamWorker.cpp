#include "AIStreamWorker.h"
#include <curl/curl.h>
#include "json.hpp"

wxDEFINE_EVENT(wxEVT_AI_STREAM_CHUNK, wxThreadEvent);

using json = nlohmann::json;

AIStreamWorkerThread::AIStreamWorkerThread(wxEvtHandler* handler,
                                         AIHandler* aiHandler,
                                         const wxString& question,
                                         const wxString& context)
    : wxThread(wxTHREAD_DETACHED),
      m_handler(handler),
      m_aiHandler(aiHandler)
{
    // Build JSON request with streaming flag
    std::string system_prompt = "You are the best terminal expert in the world integrated into a person's terminal. The user has provided the following terminal history as context. Use it to answer their requests and suggest commands.";

    std::string user_content = "--- Terminal Context ---\n" +
                               std::string(context.mb_str()) +
                               "\n\n--- User Question ---\n" +
                               std::string(question.mb_str());

    json body = {
        {"model", "gpt-3.5-turbo"},
        {"stream", true},
        {"messages", json::array({
            {{"role", "system"}, {"content", system_prompt}},
            {{"role", "user"},   {"content", user_content}}
        })}
    };
    m_requestJson = body.dump();
}

void AIStreamWorkerThread::PostEvent(int kind, const wxString& text)
{
    wxThreadEvent evt(wxEVT_AI_STREAM_CHUNK);
    evt.SetInt(kind);
    evt.SetString(text);
    wxQueueEvent(m_handler, evt.Clone());
}

void AIStreamWorkerThread::PostError(const wxString& text)
{
    if (m_terminalEventPosted) {
        return;
    }

    m_terminalEventPosted = true;
    PostEvent(AI_STREAM_ERROR, text);
}

void AIStreamWorkerThread::PostComplete()
{
    if (m_terminalEventPosted) {
        return;
    }

    m_terminalEventPosted = true;
    PostEvent(AI_STREAM_COMPLETE);
}

size_t AIStreamWorkerThread::CurlWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto self = static_cast<AIStreamWorkerThread*>(userdata);
    size_t total = size * nmemb;
    self->HandleBuffer(std::string(ptr, total));
    return total;
}

void AIStreamWorkerThread::HandleBuffer(const std::string& bufferPart)
{
    m_responseBody += bufferPart;
    m_accumBuffer += bufferPart;

    size_t pos;
    while ((pos = m_accumBuffer.find("\n")) != std::string::npos) {
        std::string line = m_accumBuffer.substr(0, pos);
        m_accumBuffer.erase(0, pos + 1);

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Each line starts with "data: "
        const std::string prefix = "data: ";
        if (line.rfind(prefix, 0) != 0) continue; // ignore non-data lines

        m_sawStreamEvent = true;
        std::string data = line.substr(prefix.size());
        if (data == "[DONE]") {
            PostComplete();
            continue;
        }

        try {
            json chunk = json::parse(data);
            if (chunk.contains("choices")) {
                auto& delta = chunk["choices"][0]["delta"];
                if (delta.contains("content")) {
                    std::string token = delta["content"].get<std::string>();
                    PostEvent(AI_STREAM_CHUNK, wxString::FromUTF8(token));
                }
            }
        } catch (...) {
            // ignore parse errors for individual lines
        }
    }
}

wxString AIStreamWorkerThread::BuildAPIErrorMessage(long responseCode) const
{
    std::string message;

    try {
        json responseJson = json::parse(m_responseBody);
        if (responseJson.contains("error")) {
            const json& error = responseJson["error"];
            if (error.contains("message") && error["message"].is_string()) {
                message = error["message"].get<std::string>();
            }
        }
    } catch (...) {
        // Fall back to the raw response body below.
    }

    if (message.empty() && !m_responseBody.empty()) {
        message = m_responseBody;
    }

    if (message.empty()) {
        return wxString::Format("Error: Assistant request failed with HTTP status %ld.", responseCode);
    }

    return wxString::Format("Error: Assistant request failed with HTTP status %ld: %s",
                            responseCode,
                            wxString::FromUTF8(message));
}

wxThread::ExitCode AIStreamWorkerThread::Entry()
{
    CURL* curl = curl_easy_init();
    if (!curl) {
        PostError("Error: Failed to initialize the network client.");
        return (wxThread::ExitCode)0;
    }

    struct curl_slist* headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + m_aiHandler->GetAPIKey();
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_requestJson.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(m_requestJson.size()));

    // Enable HTTP/2 if available for better performance
#ifdef CURLPIPE_MULTIPLEX
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
#endif

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        PostError(wxString::Format("Error: %s", curl_easy_strerror(res)));
    } else {
        long responseCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

        if (responseCode != 200) {
            PostError(BuildAPIErrorMessage(responseCode));
        } else if (!m_terminalEventPosted) {
            if (m_sawStreamEvent) {
                PostComplete();
            } else {
                PostError("Error: Assistant returned an unexpected response format.");
            }
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return (wxThread::ExitCode)0;
}
