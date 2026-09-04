# Music Player

A terminal-based music player built with C++.

## Features

- Play audio files from the terminal
- Navigate playlists with previous/next controls
- Pause, resume, and stop playback
- Interactive TUI built with FTXUI

## Tech Stack

- **Language:** C++17
- **Build System:** CMake
- **UI Library:** [FTXUI](https://github.com/ArthurSonzogni/FTXUI)
- **Audio Library:** [miniaudio](https://github.com/mackron/miniaudio)

## Prerequisites

- CMake 3.16 or later
- A C++17-compatible compiler
- Git

## Build Instructions

1. Clone the repository:

   ```bash
   git clone https://github.com/rinborobin/music-player.git
   cd music-player
   ```

2. Create a build directory and run CMake:

   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. Build the project:

   ```bash
   cmake --build .
   ```

4. Run the application:

   ```bash
   ./music-player
   ```

## Project Structure

```
music-player/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp              # Application entry point and UI
│   ├── audio/
│   │   ├── MusicEngine.h     # Audio playback interface
│   │   ├── MusicEngine.cpp   # Audio playback implementation
│   │   └── miniaudio.h       # Single-header audio library
│   └── data/
│       ├── Song.h            # Song data structure
│       ├── Playlist.h        # Playlist interface
│       ├── Playlist.cpp      # Playlist implementation
│       └── Queue.h           # Generic queue data structure
```

## Contributing

1. Fork the repository and create a new branch for your feature or fix.
2. Make your changes in small, focused commits.
3. Follow the existing code style.
4. Use [Conventional Commits](https://www.conventionalcommits.org/) for commit messages. Examples:
   - `feat(audio): add volume control`
   - `fix(ui): resolve playlist rendering issue`
   - `chore(build): update CMake minimum version`
5. Open a pull request with a clear description of your changes.

## Roadmap

- Integrate the `Queue` data structure for upcoming playback features such as:
  - Up-next queue management
  - Add songs to the playback queue
  - Clear or reorder the queue

## Notes

- Place audio files in a `music/` directory at the project root.
- The `music/` and `build/` directories are ignored by Git.
- The project currently uses sample song paths in `src/main.cpp`. Update them to match your local audio files.
