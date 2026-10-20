cmake_minimum_required(VERSION 3.15)
project(clipkeep VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# Enable LTO and optimizations for release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2 /GL /LTCG)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /OPT:REF,ICF")
    endif()
endif()

# Include directories (for header-only dependencies)
include_directories(${CMAKE_SOURCE_DIR}/vendor/include)

# Find dependencies
find_package(SQLite3 REQUIRED)

# Download nlohmann/json if not present
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/vendor/include/nlohmann/json.hpp")
    message(STATUS "Downloading nlohmann/json...")
    file(DOWNLOAD https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
         ${CMAKE_SOURCE_DIR}/vendor/include/nlohmann/json.hpp
         SHOW_PROGRESS)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/application.cpp
    src/database.cpp
    src/clipboard_monitor.cpp
    src/hotkey_manager.cpp
    src/main_window.cpp
    src/settings.cpp
)

# Create executable
add_executable(clipkeep WIN32 ${SOURCES})

# Link libraries
target_link_libraries(clipkeep PRIVATE 
    SQLite::SQLite3
    user32
    gdi32
    shell32
    comctl32
)

# Include directories
target_include_directories(clipkeep PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${SQLite3_INCLUDE_DIRS}
)

# Set subsystem to windows
set_target_properties(clipkeep PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# Install target
install(TARGETS clipkeep
    RUNTIME DESTINATION bin
)

# Build type definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_CONSOLE)
endif()
