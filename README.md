# Winamp Song Deleter Plugin

A Winamp plugin that adds a system tray icon for quickly deleting the currently playing song to the Recycle Bin.

## Project Structure

```
Winamp-Song-Deleter-Plugin/
├── src/
│   ├── plugin.cpp                  # Main plugin code (164 lines, clean & minimal)
│   ├── resource.h                  # Resource definitions (IDI_TRAYICON)
│   └── WinampSongDeleterPlugin.rc  # Resource file linking to icon
├── resources/
│   └── x_icon.ico              # System tray icon
├── WinampSDK/                  # Winamp SDK headers
│   ├── gen.h                   # General purpose plugin interface ✓
│   ├── wa_ipc.h                # IPC communication ✓
│   ├── ipc_pe.h                # Playlist editor IPC ✓
│   ├── DSP.H                   # (not used in the code, but kept for future features)
│   ├── IN2.H                   # (not used in the code, but kept for future features)
│   ├── OUT.H                   # (not used in the code, but kept for future features)
│   ├── wa_dlg.h                # (not used in the code, but kept for future features)
│   └── wa_hotkeys.h            # (not used in the code, but kept for future features)
├── CMakeLists.txt              # Clean build configuration
├── .gitignore                  # Excludes build dirs & IDE files
└── README.md                   # This file
```

## Features

- ✅ **System tray icon** with tooltip "Delete current song"
- ✅ **Left-click**: Deletes current song to Recycle Bin + removes from playlist
- ✅ **Respects shuffle/repeat** settings when playing next track
- ✅ **Long path support** (up to 32,767 characters)
- ✅ **100% recoverable** - all deletions go to Recycle Bin
- ✅ **Safe error handling** - graceful failures, no crashes
- ✅ **Minimal code** - only 164 lines, clean and efficient

## Technical Highlights

- **Unicode Shell API** (`SHFileOperationW`) for long path support
- **Proper file handle release** (moves to next track first, then deletes)
- **Proven playlist item removal** using `IPC_PE_DELETEINDEX`
- **Modern CMake build** system
- **Clean dependencies** - only essential Windows APIs

## Build Instructions

1. **Requirements:**
   - Visual Studio 2019+ or compatible C++ compiler
   - CMake 3.20+
   - Windows SDK

2. **Build:**
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" -A Win32
   cmake --build . --config Release
   ```

3. **Install:**
   - Copy `build/bin/gen_WinampSongDeleter.dll` to your Winamp `Plugins/` folder
   - Restart Winamp

## Usage

1. After installation, a small tray icon will appear in your system tray
2. When a song is playing in Winamp, left-click the tray icon
3. The current song will be moved to Recycle Bin and removed from playlist
4. Winamp will automatically play the next song (respecting shuffle/repeat settings)

## Build Output

- Generates `gen_WinampSongDeleter.dll` 
- Copy to Winamp's `Plugins/` folder to use
