# Contributor Guide: How the Music Player Works

This guide is for anyone joining the project who wants to understand the codebase without needing prior class context. It explains the architecture, the relationships between components, and how data flows through the application.

---

## What This Program Is

This is a terminal-based music player written in C++17. It uses:

- **[FTXUI](https://github.com/ArthurSonzogni/FTXUI)** for the interactive text user interface (TUI).
- **[miniaudio](https://github.com/mackron/miniaudio)** (single-header audio library) for audio playback.

The player loads songs into playlists, renders a TUI with playback controls, and displays synchronized lyrics from `.lrc` files.

---

## High-Level Architecture

The application is built around four main concerns:

1. **Audio Engine** — plays, pauses, resumes, and stops songs, and reports playback time.
2. **Data Layer** — stores songs, playlists, and lyrics.
3. **UI Layer** — renders the terminal interface and responds to user input.
4. **Application Entry Point (`main.cpp`)** — wires everything together.

```
┌─────────────────────────────────────────────────────────────┐
│                       main.cpp                              │
│  Creates: PlaylistManager, MusicEngine, LyricsManager, UI   │
└───────────────────────┬─────────────────────────────────────┘
                        │ uses
┌───────────────────────▼─────────────────────────────────────┐
│                    MusicPlayerUI                            │
│  Renders TUI, handles buttons, reads/writes playback state  │
└──────┬────────────────────┬─────────────────────────────────┘
       │ uses               │ uses
┌──────▼──────┐    ┌────────▼────────┐    ┌──────────────────┐
│ MusicEngine │    │ PlaylistManager │    │   LyricsManager  │
│  (audio)    │    │  (playlists)    │    │ (lyrics display) │
└──────┬──────┘    └────────┬────────┘    └──────────────────┘
       │                    │
       │ plays files from   │ manages
┌──────▼──────┐    ┌────────▼────────┐
│   miniaudio │    │    Playlist     │
│   library   │    │  (linked list   │
│             │    │   of Songs)     │
└─────────────┘    └─────────────────┘
```

---

## Component Reference

### 1. `MusicEngine` (`src/audio/`)

**Responsibility:** Low-level audio control.

- Initializes a miniaudio `ma_engine` on construction.
- Loads a file into an `ma_sound` object when `play()` is called.
- Provides `play`, `pause`, `resume`, `stop`, and `next`.
- Reports `getCurrentTime()`, `getDuration()`, and `getPlaybackProgress()`.

**Key point:** A song is loaded fresh each time `play()` is called. The engine does not hold a playlist; it only knows the file path it is currently playing.

### 2. `Song` (`src/data/Song.h`)

**Responsibility:** Plain data holder for one song.

```cpp
struct Song {
    std::string title;
    std::string artist;
    std::string filePath;
    std::string lyricPath;
    Song *next;
    Song *prev;
};
```

A `Song` is a node in a circular doubly linked list.

### 3. `Playlist` (`src/data/`)

**Responsibility:** Owns a circular doubly linked list of `Song` objects and tracks the currently selected song.

- `addSong(...)` — appends a new song to the circular list.
- `nextSong()` / `previousSong()` — move the internal `current` pointer.
- `getCurrentSong()` — returns the currently selected song.
- `getSongs()` — returns a flat `std::vector<Song *>` for UI rendering.
- `searchSongTitle(...)` / `searchSongArtist(...)` — linear searches.
- `removeSong(...)` — removes a node and updates `head`, `tail`, and `current` as needed.

**Key point:** The playlist is circular, so `nextSong()` from the last song wraps back to the first.

### 4. `PlaylistManager` (`src/data/`)

**Responsibility:** Owns multiple `Playlist` objects and tracks which playlist is currently active.

- `createPlaylist(name)` — creates a new playlist and sets it as current only if no playlist exists yet.
- `getPlaylists()` — returns all playlists.
- `getCurrentPlaylist()` — returns the active playlist.

**Key point:** Currently, creating playlists with the same name is allowed (it does not deduplicate).

### 5. `LyricsManager` (`src/data/`)

**Responsibility:** Parses `.lrc` lyric files and returns the lyric line that matches the current playback time.

- `loadLyrics(filePath)` — parses `[mm:ss.xx] lyric text` lines into a `std::vector<LyricLine>`.
- `getCurrentLyric(currentTime)` — returns the most recent lyric whose timestamp is `<= currentTime`.

**Key point:** Lyrics are independent of songs. The UI asks for the current lyric using the audio engine's reported playback time.

### 6. `MusicPlayerUI` (`src/ui/`)

**Responsibility:** The main interactive terminal interface.

- `run()` — starts the FTXUI event loop.
- Spawns a background thread that updates `playbackProgress` 10 times per second and posts a custom UI refresh event.
- Renders:
  - **Playlist panel** (left)
  - **Songs panel** (middle) with a lyrics strip at the bottom
  - **Controls** (bottom): previous, play/pause, next, progress bar, and "Now Playing"
- Handles button clicks:
  - `togglePlayPause()` — starts playback or pauses/resumes.
  - `playNext()` / `playPrevious()` — change the current song in the active playlist and start playback.

**Key point:** The UI does not own the data or the audio engine; it holds references to `MusicEngine`, `PlaylistManager`, and `LyricsManager`.

### 7. `main.cpp`

**Responsibility:** Bootstrap the application.

Current startup flow:

1. Create `PlaylistManager`.
2. Create `MusicEngine`.
3. Create `LyricsManager`.
4. Create several playlists (note: `"My Playlist"` is created twice in the current code).
5. Add a hardcoded sample song to the current playlist.
6. Load the corresponding `.lrc` file.
7. If lyrics load successfully, construct `MusicPlayerUI` and run it.

### 8. `DebugCLI` (`src/debug/`) — Testing Portal

**Responsibility:** Test features without launching the FTXUI interface.

- `src/debug/DebugCLI.h` / `.cpp` — menu-driven command-line interface.
- `src/debug_main.cpp` — entry point for the debug executable.

The debug portal lets you:

- Set up sample playlists and songs.
- Test `MusicEngine` operations: play, pause, resume, stop, next, previous.
- Inspect the current song and playlist state.
- Test `LyricsManager` parsing and lookup.
- Monitor playback progress for a few seconds.

Use this when you want to verify audio/data logic quickly without dealing with the TUI event loop.

### 9. `Logger` (`src/utils/`)

**Responsibility:** Lightweight logging helper.

- `src/utils/Logger.h` — header-only logger with four levels: `Debug`, `Info`, `Warning`, `Error`.
- Logs are written to **stderr** so they do not interfere with FTXUI's stdout rendering.
- Compile-time level filter via `LOG_LEVEL` macro (default: `0`, meaning everything is printed).

Example usage:

```cpp
#include "utils/Logger.h"

LOG_INFO("Playback started: " + song.title);
LOG_ERROR("Failed to load file: " + filePath);
```

---

## Data Flow During Playback

1. The user clicks the **play** button.
2. `MusicPlayerUI::togglePlayPause()` calls `playCurrentSong()`.
3. `playCurrentSong()` asks `PlaylistManager` for the current `Playlist`, then asks that playlist for its current `Song`.
4. `MusicPlayerUI` passes `song->filePath` to `MusicEngine::play()`.
5. `MusicEngine` initializes a miniaudio `ma_sound` from the file path and starts it.
6. In parallel, the background progress thread repeatedly calls `player.getPlaybackProgress()` and triggers UI redraws.
7. During each redraw, `renderSongs()` asks `MusicEngine` for `getCurrentTime()` and passes it to `LyricsManager::getCurrentLyric(...)` to show the matching lyric line.
8. When the user clicks **next** or **previous**, the playlist's `current` pointer moves, and the new song's file path is sent to `MusicEngine::play()`.

---

## Project File Layout

```
music-player/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── preview.png
│   └── CONTRIBUTORS_GUIDE.md   ← this file
├── music/                      ← place audio files here (gitignored)
├── build/                      ← CMake build output (gitignored)
└── src/
    ├── main.cpp
    ├── debug_main.cpp          ← entry point for the debug portal
    ├── audio/
    │   ├── MusicEngine.h
    │   ├── MusicEngine.cpp
    │   └── miniaudio.h         ← third-party audio library
    ├── data/
    │   ├── Song.h              ← song data node
    │   ├── Playlist.h / .cpp   ← circular doubly linked list
    │   ├── PlaylistManager.h / .cpp
    │   ├── LyricsManager.h / .cpp
    │   ├── Queue.h             ← empty placeholder
    │   └── Queue.cpp           ← empty placeholder
    ├── ui/
    │   ├── MusicPlayerUI.h
    │   └── MusicPlayerUI.cpp
    ├── debug/
    │   ├── DebugCLI.h          ← debug/test CLI interface
    │   └── DebugCLI.cpp
    └── utils/
        └── Logger.h            ← lightweight logging utility
```

> **Note:** `Queue.h` and `Queue.cpp` exist but are currently empty. They are placeholders for a future feature (see [Roadmap](#roadmap)). They are also not compiled into the executable yet.

---

## Build and Run

1. **Install prerequisites:** CMake 3.16+, a C++17 compiler, and Git.
2. **Place audio files** in a `music/` directory at the project root.
3. **Build:**

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

4. **Run the main app:**

```bash
./music-player
```

### Running the Debug / Testing Portal

The project builds a second executable called `music-player-debug`. Use it to test audio and data features without the TUI:

```bash
cd build
cmake --build .
./music-player-debug
```

You will see a menu like:

```
========== Music Player Debug Portal ==========
1. Setup sample playlists and songs
2. Play current song
3. Pause playback
4. Resume playback
5. Stop playback
6. Play next song
7. Play previous song
8. Show current song info
9. Test lyrics loading and lookup
10. Show playback progress for 5 seconds
0. Exit
================================================
>
```

Operation results and state are printed as timestamped log lines.

On the first build, CMake will download FTXUI automatically via `FetchContent`.

---

## Important Current Limitations

- **Hardcoded song paths:** `main.cpp` and `DebugCLI` currently load a specific song and lyric file (`Alex Crichton - What If I Call`). New contributors should know that local paths may need adjustment for the player or debug portal to work on their machine.
- **No file browser:** Songs are added programmatically, not loaded from the `music/` folder at runtime.
- **No queue logic yet:** The `Queue` files are placeholders and are not used by the UI or audio engine.
- **Playlist selection UI:** The UI renders the list of playlists but does not yet let the user switch between them interactively.
- **Memory management:** `Playlist`, `PlaylistManager`, and `MusicEngine` use raw `new`/`delete` and raw pointers. There is no smart-pointer ownership yet.

---

## Roadmap

Planned future work includes:

- Implementing a real `Queue` data structure for an "up next" playback queue.
- Adding songs to the queue, clearing it, and reordering items.
- Loading songs dynamically from the `music/` directory.

---

## How to Contribute

1. Fork or branch off the latest `main`.
2. Make small, focused changes.
3. Follow the existing code style (e.g., braces, naming).
4. Use [Conventional Commits](https://www.conventionalcommits.org/) for commit messages, for example:
   - `feat(audio): add volume control`
   - `fix(ui): resolve playlist rendering issue`
   - `chore(build): update CMake minimum version`
5. Open a pull request with a clear description.

If you are unsure where a change belongs, refer to the architecture diagram above and ask: "Does this affect audio playback, data storage, or the user interface?"
