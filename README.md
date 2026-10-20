# ClipKeep-Lite

A lightweight clipboard history manager built with pure C++ and Win32 API. No dependencies, just one executable.

## Features

- **Unlimited History**: Store unlimited clipboard entries (configurable storage limit)
- **Search & Filter**: Real-time search through clipboard history
- **Favorites & Pinning**: Mark important items to prevent auto-deletion
- **Groups**: Organize clips into custom groups
- **Keyboard Shortcuts**: Win+V to open (configurable)
- **Auto-Cleanup**: Automatic removal of old items when storage limit reached
- **Image Support**: Capture and store images from clipboard
- **Forked from Tauri version**: Pure C++ implementation inspired by clipkeep

## Comparison with Original ClipKeep

| Feature | Original (Rust/Tauri) | Lite (C++) |
|---------|---------------------|------------|
| Technology | Rust + Tauri 2 + Vue 3 | Pure C++ + Win32 |
| Build System | Cargo + Vite | CMake |
| Size | ~8 MB | < 2 MB |
| Dependencies | WebView2 runtime | None |
| Storage | SQLite (embedded) | SQLite (embedded) |
| CI/CD | GitHub Actions | GitHub Actions |

## Building

### Prerequisites
- Windows 10 or later
- Visual Studio 2019 or later with C++ desktop development workload
- CMake 3.15+

### Build Steps

```powershell
# Clone repository
git clone https://github.com/ouda-xiaopengyou/clipkeep-lite-cpp.git
cd clipkeep-lite

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -G "Visual Studio 17 2022" -A x64 ..

# Build
cmake --build . --config Release
```

The compiled executable will be in `build\Release\clipkeep.exe`.

### Cross-compilation (Linux/Mac)

Using MinGW-w64:

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
```

## Usage

1. Run `clipkeep.exe`
2. Press **Win+V** to open the clipboard history window
3. Use **↑↓** arrows to navigate, **Enter** to paste back to previous window
4. Right-click for options (favorite, pin, delete, move to group)

### Keyboard Shortcuts

- **Win+V**: Toggle clipboard history window
- **↑↓**: Navigate through list
- **Enter**: Paste selected item
- **Ctrl+1~9**: Quick paste numbered items

## Data Storage

Data is stored in `%APPDATA%\ClipKeep\`:
- `clipkeep.db` - SQLite database with all clipboard entries
- `settings.json` - User preferences
- `images/` - Stored image files

## Configuration

Edit `settings.json` to change settings:

```json
{
    "hotkey_enabled": true,
    "hotkey_id": "win+v",
    "capture_images": true,
    "remember_position": false,
    "max_storage_mb": 500,
    "theme": 0
}
```

## License

MIT License

## Acknowledgments

Inspired by the original [clipkeep](https://github.com/Fomovet/clipkeep) project (Tauri version).

---

**Note**: This is a simplified C++ reimplementation focusing on core functionality without UI framework dependencies. Full feature parity with the Tauri version would require additional implementation effort.
