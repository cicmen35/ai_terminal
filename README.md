# AI Terminal

An AI-powered terminal emulator built with C++ and wxWidgets, featuring a terminal interface and an AI assistant panel.

## Features

- Dual panel: Terminal and AI assistant.
- Standard terminal operations (`cd`, `clear` handled).
- AI Assistant (OpenAI GPT-3.5-turbo+) using terminal output as context.
- Secure API key loading via `.env` file or environment variable.
- Core logic is cross-platform (wxWidgets).

## Compatibility

- **macOS:** Developed and tested on macOS.
- **Other Platforms (Windows/Linux):** The project now uses CMake, which makes it highly portable. You will need to ensure wxWidgets and OpenSSL are installed and discoverable by CMake on your system.

## Prerequisites (macOS)

- C++17 (or newer) compiler 
- Homebrew package manager.
- Install all dependencies with one command:
  ```bash
  brew install cmake wxwidgets openssl@3
  ```

## API Key Setup

1. Create a `.env` file in the project's root directory.
2. Add your OpenAI API key to it in the following format:
   ```
   OPENAI_API_TOKEN=your_secret_key_here
   ```
    Alternatively, set the `OPENAI_API_TOKEN` environment variable.

## Building (macOS)

1. Create and navigate to a build directory:
   ```bash
   mkdir -p build && cd build
   ```
2. Run CMake to configure the project. We pass `CMAKE_PREFIX_PATH` to help it find Homebrew packages.
   ```bash
   cmake -DCMAKE_PREFIX_PATH=/opt/homebrew ..
   ```
3. Compile the project:
   ```bash
   make
   ```

## Running

1. **IMPORTANT**: The application looks for the `.env` file in the same directory as the executable. Before running, you must copy your `.env` file from the project root into the `build/` directory:
   ```bash
   cp .env build/ai-terminal.app/Contents/MacOS/
   ```
2. Execute the compiled binary from the project root:
   ```bash
   ./build/ai-terminal.app/Contents/MacOS/ai-terminal
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
