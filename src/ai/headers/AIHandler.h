#ifndef AI_HANDLER_H
#define AI_HANDLER_H

#include <wx/string.h>
#include <string>


class AIHandler {
public:
    AIHandler();
    const std::string& GetAPIKey() const { return apiKey; }

private:
    std::string apiKey;
    bool LoadAPIKey(); // Helper to load API key from .env
};

#endif // AI_HANDLER_H
