#include "DebugCLI.h"
#include "../utils/Logger.h"

#include <chrono>
#include <iostream>
#include <thread>

DebugCLI::DebugCLI()
    : shouldExit(false),
      sampleSongPath("../music/Alex Crichton - What If I Call (Lyrics).mp3"),
      sampleLyricPath("../music/Alex Crichton - What If I Call.lrc"),
      sampleTitle("What If I Call"),
      sampleArtist("Alex Crichton")
{
}

DebugCLI::~DebugCLI()
{
    player.stop();
}

void DebugCLI::run()
{
    LOG_INFO("Debug CLI started");
    LOG_INFO("Sample song path: " + sampleSongPath);
    LOG_INFO("Sample lyric path: " + sampleLyricPath);

    setupSampleData();

    while (!shouldExit)
    {
        printMenu();

        int choice = 0;
        std::cout << "> ";

        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            LOG_WARN("Invalid input. Please enter a number.");
            continue;
        }

        handleInput(choice);
    }

    LOG_INFO("Debug CLI exiting");
}

void DebugCLI::printMenu()
{
    std::cout << "\n========== Music Player Debug Portal ==========\n";
    std::cout << "1. Setup sample playlists and songs\n";
    std::cout << "2. Play current song\n";
    std::cout << "3. Pause playback\n";
    std::cout << "4. Resume playback\n";
    std::cout << "5. Stop playback\n";
    std::cout << "6. Play next song\n";
    std::cout << "7. Play previous song\n";
    std::cout << "8. Show current song info\n";
    std::cout << "9. Test lyrics loading and lookup\n";
    std::cout << "10. Show playback progress for 5 seconds\n";
    std::cout << "0. Exit\n";
    std::cout << "================================================\n";
}

void DebugCLI::handleInput(int choice)
{
    switch (choice)
    {
    case 1:
        setupSampleData();
        break;
    case 2:
        testPlay();
        break;
    case 3:
        testPause();
        break;
    case 4:
        testResume();
        break;
    case 5:
        testStop();
        break;
    case 6:
        testNext();
        break;
    case 7:
        testPrevious();
        break;
    case 8:
        printCurrentSong();
        break;
    case 9:
        testLyrics();
        break;
    case 10:
        testProgress();
        break;
    case 0:
        shouldExit = true;
        break;
    default:
        LOG_WARN("Unknown menu option: " + std::to_string(choice));
        break;
    }
}

void DebugCLI::setupSampleData()
{
    LOG_INFO("Setting up sample data...");

    playlistManager = PlaylistManager();

    playlistManager.createPlaylist("My Playlist");
    playlistManager.createPlaylist("Favorites");
    playlistManager.createPlaylist("Chill");

    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist == nullptr)
    {
        LOG_ERROR("Failed to create current playlist");
        return;
    }

    playlist->addSong(
        sampleTitle,
        sampleArtist,
        sampleSongPath,
        sampleLyricPath);

    playlist->addSong(
        "Merry Christmas, i miss you",
        "Alex Chrichton",
        "../music/Alex Crichton - Merry Christmas, i miss you (Lyrics).mp3",
        "");

    LOG_INFO("Created " + std::to_string(playlistManager.getPlaylists().size()) + " playlists");
    LOG_INFO("Added " + std::to_string(playlist->getSongs().size()) + " songs to current playlist");

    bool loaded = lyricsManager.loadLyrics(sampleLyricPath);
    if (loaded)
    {
        LOG_INFO("Lyrics loaded successfully. Lines: " + std::to_string(lyricsManager.getLyrics().size()));
    }
    else
    {
        LOG_WARN("Failed to load lyrics from: " + sampleLyricPath);
    }
}

void DebugCLI::testPlay()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist == nullptr)
    {
        LOG_ERROR("No current playlist");
        return;
    }

    Song *song = playlist->getCurrentSong();
    if (song == nullptr)
    {
        LOG_ERROR("No current song in playlist");
        return;
    }

    LOG_INFO("Attempting to play: " + song->title + " - " + song->artist);
    LOG_INFO("File path: " + song->filePath);

    if (player.play(song->filePath))
    {
        LOG_INFO("Playback started successfully");
    }
    else
    {
        LOG_ERROR("Failed to start playback. Check that the file exists: " + song->filePath);
    }
}

void DebugCLI::testPause()
{
    LOG_INFO("Pausing playback");
    player.pause();
}

void DebugCLI::testResume()
{
    LOG_INFO("Resuming playback");
    player.resume();
}

void DebugCLI::testStop()
{
    LOG_INFO("Stopping playback");
    player.stop();
}

void DebugCLI::testNext()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist == nullptr)
    {
        LOG_ERROR("No current playlist");
        return;
    }

    playlist->nextSong();
    Song *song = playlist->getCurrentSong();

    if (song != nullptr)
    {
        LOG_INFO("Moved to next song: " + song->title);
        player.play(song->filePath);
    }
    else
    {
        LOG_ERROR("No next song available");
    }
}

void DebugCLI::testPrevious()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist == nullptr)
    {
        LOG_ERROR("No current playlist");
        return;
    }

    playlist->previousSong();
    Song *song = playlist->getCurrentSong();

    if (song != nullptr)
    {
        LOG_INFO("Moved to previous song: " + song->title);
        player.play(song->filePath);
    }
    else
    {
        LOG_ERROR("No previous song available");
    }
}

void DebugCLI::printCurrentSong()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist == nullptr)
    {
        LOG_ERROR("No current playlist");
        return;
    }

    Song *song = playlist->getCurrentSong();
    if (song == nullptr)
    {
        LOG_WARN("No current song selected");
        return;
    }

    LOG_INFO("Current playlist: " + playlist->getName());
    LOG_INFO("Current song: " + song->title + " - " + song->artist);
    LOG_INFO("File path: " + song->filePath);
    LOG_INFO("Lyric path: " + (song->lyricPath.empty() ? "(none)" : song->lyricPath));
}

void DebugCLI::testLyrics()
{
    if (lyricsManager.getLyrics().empty())
    {
        LOG_WARN("No lyrics loaded. Run option 1 first.");
        return;
    }

    LOG_INFO("Lyrics loaded: " + std::to_string(lyricsManager.getLyrics().size()) + " lines");

    for (const LyricLine &line : lyricsManager.getLyrics())
    {
        if (line.timestamp >= 10.0f)
        {
            break;
        }

        LOG_INFO("[" + std::to_string(line.timestamp) + "s] " + line.text);
    }

    LOG_INFO("Current lyric at 30s: " + lyricsManager.getCurrentLyric(30.0f));
}

void DebugCLI::testProgress()
{
    LOG_INFO("Monitoring playback progress for 5 seconds...");

    for (int i = 0; i < 50; ++i)
    {
        float progress = player.getPlaybackProgress();
        float time = player.getCurrentTime();
        float duration = player.getDuration();

        std::cout << "\rTime: " << time << "s / " << duration << "s (" << (progress * 100.0f) << "%)" << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << std::endl;
    LOG_INFO("Progress monitoring complete");
}
