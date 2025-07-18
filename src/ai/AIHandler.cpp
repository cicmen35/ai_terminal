#include "AIHandler.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"      // For HTTP requests
#include "json.hpp"       // For JSON parsing
#include <fstream>               // For reading .env
#include <iostream>            
#include <cstdlib>               
#include <wx/stdpaths.h>         
#include <wx/filename.h>         

// Use nlohmann::json for convenience
using json = nlohmann::json;

AIHandler::AIHandler() {
    if (!LoadAPIKey()) {
        // handle this error more gracefully in a GUI app
        std::cerr << "Error: Failed to load OpenAI API Key." << std::endl;
        apiKey = ""; // Ensure apiKey is in a known state
    }
}

bool AIHandler::LoadAPIKey() {
    // simplified .env parser
    // expects OPENAI_API_TOKEN=your_key_here on a line in .env
    // .env file should be in the same directory as the executable, or adjust path.
    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    wxFileName exeFile(exePath);
    wxString envFilePath = exeFile.GetPath() + wxFileName::GetPathSeparator() + ".env";

    std::ifstream envFile(envFilePath.ToStdString());
    std::string line;
    if (envFile.is_open()) {
        while (std::getline(envFile, line)) {
            if (line.rfind("OPENAI_API_TOKEN=", 0) == 0) { // Check if line starts with the key
                apiKey = line.substr(std::string("OPENAI_API_TOKEN=").length());
                // Remove potential trailing newline or carriage return characters
                apiKey.erase(apiKey.find_last_not_of(" \n\r\t")+1);
                envFile.close();
                return !apiKey.empty();
            }
        }
        envFile.close();
    }
    // Fallback to environment variable if .env file method fails or key not found
    const char* env_key = std::getenv("OPENAI_API_TOKEN");
    if (env_key != nullptr) {
        apiKey = std::string(env_key);
        return !apiKey.empty();
    }
    return false;
}

wxString AIHandler::GetAIResponse(const wxString& userInput, const wxString& terminalContext) {
    if (apiKey.empty()) {
        return "Error: API key not configured. Check the openai api connection.";
    }
    std::cout << "API key successfully configured.";
    // Use SSLClient for HTTPS connections
    httplib::SSLClient cli("api.openai.com");
    cli.set_default_headers({{"Authorization", "Bearer " + apiKey}});
    // Server certificate verification is on by default with SSLClient

    // Construct a system message to guide the AI
    std::string system_prompt = "You are the best terminal expert in the world integrated into a person's terminal. The user has provided the following terminal history as context. Use it to answer their requests and suggest commands.";

    // Combine the context and the user's question
    std::string user_content = "--- Terminal Context ---\n" + terminalContext.ToStdString() + 
                               "\n\n--- User Question ---\n" + userInput.ToStdString();

    json requestBody = {
        {"model", "gpt-3.5-turbo"},
        {"messages", json::array({
            {{"role", "system"}, {"content", system_prompt}},
            {{"role", "user"}, {"content", user_content}}
        })}
    };

    auto res = cli.Post("/v1/chat/completions", requestBody.dump(), "application/json");

    if (res && res->status == 200) {
        try {
            json responseJson = json::parse(res->body);
            if (responseJson.contains("choices") && responseJson["choices"].is_array() && !responseJson["choices"].empty()) {
                if (responseJson["choices"][0].contains("message") && responseJson["choices"][0]["message"].contains("content")) {
                    std::string content = responseJson["choices"][0]["message"]["content"].get<std::string>();
                    return wxString(content);
                }
            }
            return "Error: Unexpected JSON structure in API response.";
        } catch (json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            return wxString::Format("Error: Could not parse API response - %s", e.what());
        }
    } else {
        std::string errorMsg = "Error: API request failed.";
        if (res) {
            errorMsg += " Status: " + std::to_string(res->status);
            errorMsg += " Body: " + res->body;
            std::cerr << "API Error: " << res->status << " Body: " << res->body << std::endl;
        }
        return wxString(errorMsg);
    }
}
