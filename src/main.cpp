#include "audio/MusicEngine.h"
#include "data/Playlist.h"
#include "data/PlaylistManager.h"
#include "ui/MusicPlayerUI.h"
#include "data/LyricsManager.h"

#include <filesystem>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <climits>
#include <unistd.h>
#endif

namespace
{

std::filesystem::path getExecutablePath()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
        return {};
    return std::filesystem::path(buffer);
#else
    char buffer[PATH_MAX];
    ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (length == -1)
        return {};
    buffer[length] = '\0';
    return std::filesystem::path(buffer);
#endif
}

std::filesystem::path findMusicDirectory()
{
    std::vector<std::filesystem::path> candidates;

    auto exePath = getExecutablePath();
    if (!exePath.empty())
    {
        auto exeDir = exePath.parent_path();
        candidates.push_back(exeDir / "music");
        candidates.push_back(exeDir / ".." / "music");
    }

    candidates.push_back(std::filesystem::current_path() / "music");

    for (const auto &candidate : candidates)
    {
        try
        {
            auto canonical = std::filesystem::canonical(candidate);
            if (std::filesystem::is_directory(canonical))
            {
                return canonical;
            }
        }
        catch (...)
        {
            // Candidate does not exist or cannot be resolved; try the next one.
        }
    }

    // Fallback: prefer the executable's sibling ../music directory.
    if (!exePath.empty())
    {
        return exePath.parent_path() / ".." / "music";
    }

    return std::filesystem::current_path() / "music";
}

std::string musicFile(const std::filesystem::path &musicDir,
                      const std::string &fileName)
{
    return (musicDir / fileName).string();
}

} // namespace

int main()
{
    PlaylistManager playlistManager;
    MusicEngine player;

    LyricsManager lyricsManager;

    Playlist *myPlaylist =
        playlistManager.createPlaylist("My Playlist");

    Playlist *favorites =
        playlistManager.createPlaylist("Favorites");

    Playlist *chill =
        playlistManager.createPlaylist("Chill");

    std::filesystem::path musicDir = findMusicDirectory();
    std::cout << "Music directory: " << musicDir << std::endl;

    Playlist *playlist = playlistManager.getCurrentPlaylist();

    if (playlist != nullptr)
    {
        playlist->addSong(
            "What If I Call",
            "Alex Crichton",
            musicFile(musicDir, "Alex Crichton - What If I Call (Lyrics).mp3"),
            musicFile(musicDir, "Alex Crichton - What If I Call.lrc"));

        playlist->addSong(
            "Shape of My Heart",
            "Backstreet Boys",
            musicFile(musicDir, "Backstreet Boys - Shape of My Heart (Lyrics).mp3"),
            musicFile(musicDir, "Backstreet Boys - Shape of My Heart.lrc"));

        favorites->addSong(
            "Shape of My Heart",
            "Backstreet Boys",
            musicFile(musicDir, "Backstreet Boys - Shape of My Heart (Lyrics).mp3"),
            musicFile(musicDir, "Backstreet Boys - Shape of My Heart.lrc"));

        chill->addSong(
            "Merry Christmas, i miss you",
            "Alex Crichton",
            musicFile(musicDir, "Alex Crichton - Merry Christmas, i miss you (Lyrics).mp3"),
            "");

        bool loaded = lyricsManager.loadLyrics(
            musicFile(musicDir, "Alex Crichton - What If I Call.lrc"));

        if (!loaded)
        {
            std::cout << "Failed to load lyrics.\n";
        }
        else
        {
            std::cout << "Lyrics loaded successfully!\n";

            auto lyrics = lyricsManager.getLyrics();

            MusicPlayerUI ui(player, playlistManager, lyricsManager);
            ui.run();

            return 0;
        }
    }
}
