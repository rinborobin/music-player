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
└──────┬──────────┬───────────────┬────────────────────────────┘
       │ uses     │ uses          │ uses
┌──────▼──────┐ ┌─▼────────────┐ ┌▼─────────────┐ ┌───────────▼──┐
│ MusicEngine │ │PlaylistManager│ │ LyricsManager│ │    Queue     │
│  (audio)    │ │  (playlists)  │ │(lyrics disp.)│ │ (up next)    │
└──────┬──────┘ └───────┬────────┘ └──────────────┘ └──────────────┘
       │                │
       │ plays files from│ manages
┌──────▼──────┐ ┌────────▼────────┐
│   miniaudio │ │    Playlist     │
│   library   │ │  (linked list   │
│             │ │   of Songs)     │
└─────────────┘ └─────────────────┘
```

---

## Component Reference

### 1. `MusicEngine` (`src/audio/`)

**Responsibility:** Low-level audio control.

- Initializes a miniaudio `ma_engine` on construction.
- Loads a file into an `ma_sound` object when `play()` is called.
- Provides `play`, `pause`, `resume`, `stop`, and `next`.
- Reports `getCurrentTime()`, `getDuration()`, and `getPlaybackProgress()`.
- Registers an end-of-sound callback so the UI can auto-advance when a track finishes.
- Exposes `consumeFinished()` to detect a natural song end.

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
- `selectSong(index)` — sets the current song by index.
- `getCurrentSong()` — returns the currently selected song.
- `getSongs()` — returns a flat `std::vector<Song *>` for UI rendering.
- `searchSongTitle(...)` / `searchSongArtist(...)` — linear searches.
- `removeSong(...)` — removes a node and updates `head`, `tail`, and `current` as needed.

**Key point:** The playlist is circular, so `nextSong()` from the last song wraps back to the first.

### 4. `PlaylistManager` (`src/data/`)

**Responsibility:** Owns multiple `Playlist` objects and tracks which playlist is currently active.

- `createPlaylist(name)` — creates a new playlist, returns a pointer to it, and sets it as current only if no playlist exists yet.
- `getPlaylists()` — returns all playlists.
- `getCurrentPlaylist()` — returns the active playlist.
- `selectPlaylist(index)` — switches the active playlist by index.

**Key point:** Currently, creating playlists with the same name is allowed (it does not deduplicate).

### 5. `LyricsManager` (`src/data/`)

**Responsibility:** Parses `.lrc` lyric files and returns the lyric line that matches the current playback time.

- `loadLyrics(filePath)` — parses `[mm:ss.xx] lyric text` lines into a `std::vector<LyricLine>`.
- `getCurrentLyric(currentTime)` — returns the most recent lyric whose timestamp is `<= currentTime`.

**Key point:** Lyrics are independent of songs. The UI asks for the current lyric using the audio engine's reported playback time.

### 6. `Queue` (`src/data/`)

**Responsibility:** A playback "up next" queue.

- `addSongToQueue(Song *)` — adds a song to the back of the queue.
- `deQueue()` — removes and returns the front song.
- `peek()` — returns the front song without removing it.
- `clearQueue()` — empties the queue.
- `isEmpty()` / `getQueueSize()` — queue state.
- `removeQueueAt(index)` — removes a song at a specific position.
- `getQueueSongs()` — returns a vector of queued songs for UI rendering without dequeuing.

**Key point:** The queue stores `Song *` pointers but does not own the songs (the `Playlist` owns them). It is wired into the main TUI as the **Up Next** panel and is also used by the debug portal.

### 7. `MusicPlayerUI` (`src/ui/`)

**Responsibility:** The main interactive terminal interface.

- `run()` — starts the FTXUI event loop.
- Spawns a background thread that updates `playbackProgress` 10 times per second, detects when the current song ends, and posts custom UI refresh events.
- Renders:
  - **Playlist panel** (left) — click to switch playlists.
  - **Songs panel** (center/right) — scrollable song list with an **Up Next** queue box at the bottom.
  - **Controls** (bottom): previous, play/pause, next, **Add** (enqueue selected song), progress bar, and "Now Playing".
- Handles user actions:
  - Click a song to play it.
  - Click a playlist to browse it (does not affect the queue or currently playing song).
  - Click **Add** to enqueue the selected song.
  - Click **⏭** to play the next queued song, or the next playlist song if the queue is empty.
  - `togglePlayPause()` — starts playback or pauses/resumes.
  - `playNext()` / `playPrevious()` — change the current song.
- Auto-advances to the next queued/playlist song when playback ends naturally.

**Key point:** The UI does not own the data or the audio engine; it holds references to `MusicEngine`, `PlaylistManager`, `LyricsManager`, and owns a `Queue`.

### 8. `main.cpp`

**Responsibility:** Bootstrap the application.

Current startup flow:

1. Create `PlaylistManager`, `MusicEngine`, and `LyricsManager`.
2. Create several playlists (`My Playlist`, `Favorites`, `Chill`).
3. Resolve the `music/` directory relative to the executable (falls back to current working directory).
4. Add hardcoded sample songs to each playlist.
5. Load the corresponding `.lrc` file.
6. If lyrics load successfully, construct `MusicPlayerUI` and run it.

### 9. `DebugCLI` (`src/debug/`) — Testing Portal

**Responsibility:** Test features without launching the FTXUI interface.

- `src/debug/DebugCLI.h` / `.cpp` — menu-driven command-line interface.
- `src/debug_main.cpp` — entry point for the debug executable.

The debug portal lets you:

- Set up sample playlists and songs.
- Test `MusicEngine` operations: play, pause, resume, stop, next, previous.
- Inspect the current song and playlist state.
- Test `LyricsManager` parsing and lookup.
- Monitor playback progress for a few seconds.
- Test the `Queue`: enqueue, dequeue, peek, display, clear, move, and remove.

Use this when you want to verify audio/data logic quickly without dealing with the TUI event loop.

### 10. `Logger` (`src/utils/`)

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

1. The user clicks a song or the **play** button.
2. `MusicPlayerUI` selects the song and calls `playCurrentSong()` / `playSong(Song *)`.
3. `MusicPlayerUI` passes `song->filePath` to `MusicEngine::play()`.
4. `MusicEngine` initializes a miniaudio `ma_sound` from the file path, registers an end-of-sound callback, and starts it.
5. In parallel, the background progress thread repeatedly calls `player.getPlaybackProgress()` and triggers UI redraws.
6. During each redraw, `renderNowPlaying()` asks `MusicEngine` for `getCurrentTime()` and passes it to `LyricsManager::getCurrentLyric(...)` to show the matching lyric line.
7. When the user clicks **next**, `playNext()` checks the `Queue` first. If it is not empty, the front song is dequeued and played; otherwise the playlist's `current` pointer moves forward.
8. When the current song ends naturally, the end-of-sound callback sets a flag; the progress thread detects it and calls `playNext()` to continue playback.

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
    │   ├── Queue.h             ← playback queue
    │   └── Queue.cpp           ← playback queue implementation
    ├── ui/
    │   ├── MusicPlayerUI.h
    │   └── MusicPlayerUI.cpp
    ├── debug/
    │   ├── DebugCLI.h          ← debug/test CLI interface
    │   └── DebugCLI.cpp
    └── utils/
        └── Logger.h            ← lightweight logging utility
```

> **Note:** `Queue` is implemented as a playback "up next" queue. It is wired into the main TUI as the **Up Next** panel and is also used by the debug portal.

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
11. Add current song to playback queue
12. Play next song from queue
13. Peek queue front
14. Display queue
15. Clear queue
16. Move song in queue
17. Remove song from queue
0. Exit
================================================
>
```

Operation results and state are printed as timestamped log lines.

On the first build, CMake will download FTXUI automatically via `FetchContent`.

---

## Important Current Limitations

- **No file browser:** Songs are added programmatically in `main.cpp`, not loaded from the `music/` folder at runtime.
- **Memory management:** `Playlist`, `PlaylistManager`, `Queue`, and `MusicEngine` use raw `new`/`delete` and raw pointers. There is no smart-pointer ownership yet.
- **Single audio device:** `MusicEngine` uses the default miniaudio device with no volume or device-selection controls.

---

## Roadmap

Planned future work includes:

- Loading songs dynamically from the `music/` directory.
- Volume control and audio device selection.
- Shuffle / repeat playback modes.

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
