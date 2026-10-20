# ClipKeep-Lite Setup Guide

## Overview

This repository contains a lightweight C++ implementation of clipboard history management inspired by the original Tauri-based clipkeep project. The C++ version uses pure Win32 API with no external dependencies beyond SQLite3.

## Project Structure

```
clipkeep-lite/
├── src/                          # Source code
│   ├── main.cpp                 # Application entry point
│   ├── application.cpp/h        # Core application class
│   ├── database.cpp/h           # SQLite database operations
│   ├── clipboard_monitor.cpp/h  # Clipboard change detection
│   ├── hotkey_manager.cpp/h     # Global hotkey handling (Win+V)
│   ├── main_window.cpp/h        # Main UI window
│   ├── settings.cpp/h           # Settings persistence
│   └── utilities.h              # Utility functions
├── .github/workflows/           # GitHub Actions CI/CD
│   ├── build.yml               # Build on push/pull request
│   └── release.yml             # Build + release on tag
├── CMakeLists.txt               # CMake build configuration
├── README.md                    # Project documentation
├── BUILD.md                     # Detailed build instructions
├── SETUP.md                     # This file
└── deploy.ps1                   # PowerShell deployment script
```

## Quick Start

### 1. Prerequisites

- Windows 10 or later
- Visual Studio 2019/2022 (Build Tools or full IDE)
- CMake 3.15+
- SQLite3 development libraries (included via NuGet in VS)

### 2. Clone Repository

```bash
git clone https://github.com/YOUR_USERNAME/clipkeep-lite.git
cd clipkeep-lite
```

### 3. Build Locally

```powershell
mkdir build
cd build

cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release

# Result: build\Release\clipkeep.exe
```

### 4. Deploy to GitHub

#### Option A: Using the Deployment Script

```powershell
.\deploy.ps1 -GitHubToken "ghp_..."
```

#### Option B: Manual Steps

```powershell
# Initialize git (if not done already)
git init

# Configure git user
git config user.name "Your Name"
git config user.email "your@email.com"

# Add all files
git add .
git commit -m "Initial commit"

# Create GitHub repository (via web UI), then:
git remote add origin https://github.com/YOUR_USERNAME/clipkeep-lite.git
git branch -M main
git push -u origin main
```

### 5. Create First Release

After pushing to GitHub:

```bash
# Create a tag
git tag v1.0.0
git push origin v1.0.0

# GitHub Actions will automatically:
# 1. Build the project on windows-latest
# 2. Create a release with the exe
# 3. Add release notes
```

## Key Features Implemented

### Core Functionality

| Feature | Status | Description |
|---------|--------|-------------|
| Clipboard Monitoring | ✅ | Detects text/image copy events |
| Duplicate Detection | ✅ | Uses hash comparison to prevent duplicates |
| Database Storage | ✅ | SQLite for persistent storage |
| Text Support | ✅ | UTF-8 text capture and retrieval |
| Image Support | ⚠️  Basic | Framework ready, needs GDI+ integration |
| Search | ⚠️  Pending | UI search filter not implemented |
| Groups | ⚠️  Partial | Database schema ready, UI pending |
| Favorites/Pin | ⚠️  Partial | Schema ready, UI pending |
| Auto-Cleanup | ✅ | Enforces storage limits automatically |

### UI Components

| Component | Status | Notes |
|-----------|--------|-------|
| Main Window | ✅ | Basic window created |
| ListView | ⚠️  Stub | Full implementation needed |
| Keyboard Shortcuts | ⚠️  Basic | Hotkey registered, message handler pending |
| Tray Icon | ❌ | Not yet implemented |
| Context Menu | ❌ | Not yet implemented |

### Configuration

| Setting | Default | Type |
|---------|---------|------|
| `hotkey_id` | "win+v" | String |
| `capture_images` | true | Boolean |
| `max_storage_mb` | 500 | Integer (MB) |
| `remember_position` | false | Boolean |
| `theme` | 0 (System) | Integer (0=System, 1=Light, 2=Dark) |

## Building Details

### Dependencies

The build system handles most dependencies automatically:

1. **SQLite3**: Use pre-built binaries from sqlite.org
2. **nlohmann/json**: Downloaded automatically during CMake configure
3. **Windows SDK**: Included with Visual Studio

### CMake Variables

| Variable | Description | Recommended Value |
|----------|-------------|-------------------|
| `CMAKE_BUILD_TYPE` | Build configuration | Release |
| `CMAKE_INSTALL_PREFIX` | Install location | N/A (single exe) |
| `ENABLE_TESTS` | Include test builds | OFF |

### Compiler Flags (Release Mode)

```cmake
-O2          # Optimize for speed
/GL          # Link-time code generation
/LTCG        # Whole program optimization
/OPT:REF     # Remove unused data
/OPT:ICF     # Inline function merging
```

Result: Single ~1-2 MB executable.

## Continuous Integration

### GitHub Actions Workflows

**build.yml** (runs on every PR):
- Builds on `windows-latest`
- Runs basic tests (when added)
- Uploads artifact for inspection

**release.yml** (runs on tags):
- Creates tagged releases
- Publishes `.exe` binary
- Generates release notes

### Local Testing Workflow

Simulate CI locally:

```powershell
# Clean build
Remove-Item build -Recurse -Force
mkdir build
cd build

# Configure like CI does
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release

# Verify executable runs
.\Release\clipkeep.exe
```

## Next Steps for Completion

To achieve full feature parity with original clipkeep:

1. **Implement ListView Control**
   - Display clipboard entries
   - Support keyboard navigation
   - Highlight selected item

2. **Add Context Menu**
   - Right-click menu
   - Actions: Paste, Copy, Favorite, Pin, Delete, Move to Group

3. **Tray Icon**
   - System tray notification
   - Quick access menu
   - Minimize to tray option

4. **Enhance Search**
   - Real-time filtering
   - Case-insensitive matching
   - Wildcard support

5. **Image Handling**
   - Proper bitmap conversion
   - Thumbnail generation
   - JPEG/PNG compression

6. **LAN Transfer** (Advanced)
   - Socket communication
   - Peer discovery
   - Encryption

## Troubleshooting

### Common Issues

**Issue**: CMake can't find SQLite3
```
Solution: Download SQLitewin64.zip and extract to vendor/sqlite3/
Then specify paths: cmake -DSQLite3_INCLUDE_DIR="vendor/sqlite3/include" ...
```

**Issue**: Missing nlohmann/json.hpp
```
Solution: The download step should run automatically. If not:
wget https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -o vendor/include/nlohmann/json.hpp
```

**Issue**: Build fails with LNK2019 errors
```
Solution: Ensure these libraries are linked in CMakeLists.txt:
user32, gdi32, shell32, comctl32
```

**Issue**: Window doesn't appear
```
Solution: Check for error dialog on startup. Might be missing common controls initialization.
Run from command line: .\Release\clipkeep.exe
```

## Contributing

Contributions welcome! Areas needing attention:

- UI rendering improvements
- Performance optimizations
- Additional features (LAN transfer, advanced search)
- Documentation improvements

## License

MIT License - see LICENSE file for details.

Inspired by [clipkeep](https://github.com/Fomovet/clipkeep) - MIT licensed.

---

**Version**: 1.0.0  
**Last Updated**: 2026-10-20
