#include "AIHandler.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <wx/filename.h>
#include <wx/stdpaths.h>

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
