# Build Instructions for ClipKeep-Lite

## Prerequisites

### Windows (Recommended)
1. **Visual Studio 2019/2022** with:
   - C++ desktop development workload
   - Windows 10/11 SDK
   
2. **CMake 3.15+**
   ```powershell
   winget install Kitware.CMake
   ```

3. **SQLite3** (Windows pre-built binaries from sqlite.org)
   - Download SQLitewin64.zip from https://www.sqlite.org/download.html
   - Extract to `vendor/sqlite3/`

### Linux/Mac (Cross-compilation)
```bash
# Ubuntu/Debian
sudo apt-get install cmake g++ libsqlite3-dev

# macOS
brew install cmake sqlite3
```

## Build Process

### Option 1: Visual Studio (Windows)

```powershell
cd clipkeep-lite

# Create build directory
mkdir build
cd build

# Configure for MSVC
cmake -G "Visual Studio 17 2022" -A x64 ..

# Build Release version
cmake --build . --config Release

# Result: build\Release\clipkeep.exe
```

### Option 2: MinGW-w64 (Windows/Linux)

```bash
mkdir build && cd build

# Configure
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..

# Build
make

# Result: clipkeep.exe
```

### Option 3: Ninja (Fastest)

```powershell
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja

# Result: Release/clipkeep.exe
```

## Build Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `-DCMAKE_BUILD_TYPE` | Build type (Debug/Release) | Release |
| `-DCMAKE_INSTALL_PREFIX` | Installation directory | N/A |
| `-DENABLE_TESTS` | Build tests | OFF |

## Testing on GitHub Actions

The CI workflow automatically builds on every push to main branch:
- Builds on `windows-latest` runner
- Uses MSVC compiler
- Outputs `clipkeep.exe` as artifact

To test locally with similar setup, use:

```powershell
# Simulate GitHub Actions environment
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Distribution

After successful build:

1. Copy `clipkeep.exe` to target machine
2. No additional dependencies required!
3. First run will create `%APPDATA%\ClipKeep\` directory

## Troubleshooting

### SQLite3 Not Found
Add include/lib paths manually:
```powershell
cmake -DSQLite3_INCLUDE_DIR="path\to\sqlite" ^
      -DSQLite3_LIBRARY="path\to\sqlite3.lib" ..
```

### Missing Headers
Ensure vendor dependencies are downloaded:
```bash
git submodule update --init --recursive
```

### Link Errors
Verify all libraries are linked in CMakeLists.txt.

## Post-Build Verification

Check build succeeded:
```powershell
cd build\Release
.\clipkeep.exe
```

Should show window without errors if built correctly.
