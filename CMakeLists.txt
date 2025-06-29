cmake_minimum_required(VERSION 3.10)

project(AITerminal VERSION 1.0)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find wxWidgets
# On macOS, you might need to set wxWidgets_ROOT_DIR if CMake doesn't find it automatically
# e.g., set(wxWidgets_ROOT_DIR "/opt/homebrew/opt/wxwidgets") or wherever brew installs it.
# Alternatively, wx-config can often provide the necessary paths.
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# Find OpenSSL
# CMake should find OpenSSL installed by Homebrew automatically on macOS.
# If not, you might need to hint CMAKE_PREFIX_PATH or OpenSSL_ROOT_DIR.
# e.g. set(OpenSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
find_package(OpenSSL REQUIRED)

# Add include directories for your project headers and libs
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libs
    ${OpenSSL_INCLUDE_DIR} # Add OpenSSL include directory
)

# Define the executable and its source files
add_executable(ai-terminal
    src/main.cpp
    src/mainWindow.cpp
    src/ai_handler.cpp
)

# Link libraries
target_link_libraries(ai-terminal
    PRIVATE
    ${wxWidgets_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Add wxWidgets compile flags
target_compile_options(ai-terminal PRIVATE ${wxWidgets_CXX_FLAGS})

# --- Optional: For macOS to create a proper .app bundle ---
if(APPLE)
    set_target_properties(ai-terminal PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        # You might need to create a simple Info.plist.in if you want a .app
        # For now, this will make it a command-line app that can still use GUI.
    )
endif()

message(STATUS "wxWidgets Libraries: ${wxWidgets_LIBRARIES}")
message(STATUS "wxWidgets Include Dirs: ${wxWidgets_INCLUDE_DIRS}")
message(STATUS "wxWidgets CXX Flags: ${wxWidgets_CXX_FLAGS}")
message(STATUS "OpenSSL Include Dir: ${OpenSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL Libraries: ${OpenSSL_LIBRARIES}")
