# AI Terminal

An AI-assisted command runner built with C++ and wxWidgets. The app shows a terminal-style command pane beside an assistant pane that can answer questions using recent terminal output as context.

## Features

- Split view with a terminal-style pane and an assistant pane.
- Built-in handling for `cd` and `clear`.
- File-path autocomplete and command history in the terminal input.
- Streaming assistant replies powered by the OpenAI Chat Completions API.
- API key loading from a `.env` file next to the app binary or from `OPENAI_API_TOKEN`.

## Compatibility

- **macOS:** Developed on macOS.
- **Other Platforms (Windows/Linux):** You will need wxWidgets and libcurl installed and discoverable by CMake.

## Prerequisites (macOS)

- C++17 (or newer) compiler
- Homebrew package manager.
- Install all dependencies with one command:
  ```bash
  brew install cmake wxwidgets
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
  cmake --build .
  ```

## Running

1. Copy your `.env` file next to the built executable:
   ```bash
   cp .env build/ai-terminal.app/Contents/MacOS/
   ```
2. Execute the compiled binary from the project root:
   ```bash
   ./build/ai-terminal.app/Contents/MacOS/ai-terminal
   ```

## Limitations

- This is not a PTY-backed terminal emulator, so interactive full-screen tools such as `vim` and `nano` are intentionally blocked.
- Only one shell command runs at a time.
- Only one assistant response streams at a time.
- There is currently no automated test suite.

## Project Structure

- `src/ui/`: Window, pane, and terminal input logic.
- `src/ai/`: API key loading and assistant streaming worker.
- `src/libs/`: Vendored third-party headers.
