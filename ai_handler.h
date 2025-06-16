#ifndef AI_HANDLER_H
#define AI_HANDLER_H

#include <wx/string.h>
#include <string>

// Forward declaration for httplib client to avoid including the full header here if possible
// However, for simplicity and since it's header-only, we might include it directly.
// For now, let's assume we'll include it in the .cpp file or here if needed for member types.

class AIHandler {
public:
    AIHandler();
    wxString GetAIResponse(const wxString& userInput, const wxString& terminalContext);

private:
    std::string apiKey;
    bool LoadAPIKey(); // Helper to load API key from .env
};

#endif // AI_HANDLER_H
