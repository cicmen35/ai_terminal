# AI Terminal

An AI-powered terminal emulator built with C++ and wxWidgets, featuring a terminal interface and an AI assistant panel.

## Features

- Dual panel: Terminal and AI assistant.
- Standard terminal operations (`cd`, `clear` handled).
- AI Assistant (OpenAI GPT-3.5-turbo+) using terminal output as context.
- Secure API key loading via `.env` file or environment variable.
- Core logic is cross-platform (wxWidgets).

## Compatibility

- **macOS:** Currently developed and tested on macOS. Build instructions below are for macOS with Homebrew.
- **Other Platforms (Windows/Linux):** The core is portable. For a full cross-platform build, migrating to CMake and providing platform-specific dependency (wxWidgets, OpenSSL) instructions is recommended.

## Prerequisites (macOS)

- C++11 (or newer) compiler.
- wxWidgets: `brew install wxwidgets`
- OpenSSL v3: `brew install openssl@3` (for OpenAI API HTTPS)

## Configuration: OpenAI API Key

1.  Create a `.env` file in the project root.
2.  Add your API key:
    ```
    OPENAI_API_TOKEN=your_actual_api_key_here
    ```
    Alternatively, set the `OPENAI_API_TOKEN` environment variable.

## Building (macOS)

From the project root directory:

```bash
g++ -std=c++11 src/main.cpp src/mainWindow.cpp src/ai_handler.cpp -o ai-terminal `wx-config --cppflags --libs` -I./src/headers -I./src/libs -I/opt/homebrew/opt/openssl@3/include -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
```

## Running

From the project root directory:

```bash
./ai-terminal
```

## Project Structure

- `src/`: Main C++ source files (`.cpp`).
- `src/headers/`: Project header files (`.h`).
- `src/libs/`: Third-party header-only libraries.
- `.env`: OpenAI API key storage.
- `.gitignore`: Git ignore rules.


## Potential Future Enhancements

- CMake build system for cross-platform support.
- Windows/Linux build instructions.
- Command history & tab completion.
- Improved AI context handling.
