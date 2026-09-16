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
    std::cout << "11. Add current song to playback queue\n";
    std::cout << "12. Play next song from queue\n";
    std::cout << "13. Peek queue front\n";
    std::cout << "14. Display queue\n";
    std::cout << "15. Clear queue\n";
    std::cout << "16. Move song in queue\n";
    std::cout << "17. Remove song from queue\n";
    std::cout << "18. Sort playlist by title (debug)\n";
    std::cout << "19. Sort playlist by artist (debug)\n";
    std::cout << "20. Display all songs in current playlist\n";
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
    case 11:
        queueAddCurrentSong();
        break;
    case 12:
        queuePlayNext();
        break;
    case 13:
        queuePeek();
        break;
    case 14:
        queueDisplay();
        break;
    case 15:
        queueClear();
        break;
    case 16:
        queueMove();
        break;
    case 17:
        queueRemove();
        break;
    case 18:
        testSortByTitle();
        break;
    case 19:
        testSortByArtist();
        break;
    case 20:
        displayAllSongs();
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

    playlist->addSong(
        "Shape of My Heart",
        "Backstreet Boys",
        "../music/Backstreet Boys - Shape of My Heart (Lyrics).mp3",
        "../music/Backstreet Boys - Shape of My Heart.lrc");

    playlist->addSong(
        "Test Track One",
        "Zebra Band",
        "../music/test1.mp3",
        "");

    playlist->addSong(
        "Ambient Intro",
        "Mellow Mind",
        "../music/test2.mp3",
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

void DebugCLI::testSortByTitle()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist == nullptr)
    {
        LOG_ERROR("No current playlist");
        return;
    }

    std::vector<Song *> before = playlist->getSongs();
    if (before.empty())
    {
        LOG_WARN("Playlist is empty; nothing to sort");
        return;
    }

    LOG_INFO("[sortByTitle] Playlist BEFORE sort:");
    for (size_t i = 0; i < before.size(); ++i)
    {
        LOG_INFO("  " + std::to_string(i + 1) + ". " + before[i]->title + " - " + before[i]->artist);
    }

    LOG_INFO("[sortByTitle] Calling Playlist::sortByTitle()...");
    playlist->sortByTitle();
    LOG_INFO("[sortByTitle] Returned from Playlist::sortByTitle().");

    std::vector<Song *> after = playlist->getSongs();
    LOG_INFO("[sortByTitle] Playlist AFTER sort:");
    for (size_t i = 0; i < after.size(); ++i)
    {
        LOG_INFO("  " + std::to_string(i + 1) + ". " + after[i]->title + " - " + after[i]->artist);
    }

    LOG_INFO("[sortByTitle] Note: sortByTitle() is currently not implemented, so order should be unchanged.");
}

void DebugCLI::testSortByArtist()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist == nullptr)
    {
        LOG_ERROR("No current playlist");
        return;
    }

    std::vector<Song *> before = playlist->getSongs();
    if (before.empty())
    {
        LOG_WARN("Playlist is empty; nothing to sort");
        return;
    }

    LOG_INFO("[sortByArtist] Playlist BEFORE sort:");
    for (size_t i = 0; i < before.size(); ++i)
    {
        LOG_INFO("  " + std::to_string(i + 1) + ". " + before[i]->artist + " - " + before[i]->title);
    }

    LOG_INFO("[sortByArtist] Calling Playlist::sortByArtist()...");
    playlist->sortByArtist();
    LOG_INFO("[sortByArtist] Returned from Playlist::sortByArtist().");

    std::vector<Song *> after = playlist->getSongs();
    LOG_INFO("[sortByArtist] Playlist AFTER sort:");
    for (size_t i = 0; i < after.size(); ++i)
    {
        LOG_INFO("  " + std::to_string(i + 1) + ". " + after[i]->artist + " - " + after[i]->title);
    }

    LOG_INFO("[sortByArtist] Note: sortByArtist() is currently not implemented, so order should be unchanged.");
}

void DebugCLI::displayAllSongs()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist == nullptr)
    {
        LOG_ERROR("No current playlist");
        return;
    }

    std::vector<Song *> songs = playlist->getSongs();
    if (songs.empty())
    {
        LOG_WARN("Current playlist is empty");
        return;
    }

    LOG_INFO("Songs in '" + playlist->getName() + "':");
    for (size_t i = 0; i < songs.size(); ++i)
    {
        LOG_INFO("  " + std::to_string(i + 1) + ". " + songs[i]->title + " - " + songs[i]->artist);
    }
}

void DebugCLI::queueAddCurrentSong()
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
        LOG_ERROR("No current song to add to queue");
        return;
    }

    // playbackQueue.enqueue(song);
    LOG_WARN("Queue is not wired up in this debug build");
}

void DebugCLI::queuePlayNext()
{
    // Song *song = playbackQueue.dequeue();
    // if (song == nullptr)
    // {
    //     LOG_WARN("Queue is empty");
    //     return;
    // }

    // LOG_INFO("Playing next from queue: " + song->title);
    // player.play(song->filePath);
    LOG_WARN("Queue is not wired up in this debug build");
}

void DebugCLI::queuePeek()
{
    // Song *song = playbackQueue.peek();
    // if (song == nullptr)
    // {
    //     LOG_INFO("Queue is empty");
    //     return;
    // }

    // LOG_INFO("Next in queue: " + song->title + " - " + song->artist);
    LOG_WARN("Queue is not wired up in this debug build");
}

void DebugCLI::queueDisplay()
{
    // playbackQueue.display();
    LOG_WARN("Queue is not wired up in this debug build");
}

void DebugCLI::queueClear()
{
    // playbackQueue.clear();
    LOG_WARN("Queue is not wired up in this debug build");
}

void DebugCLI::queueMove()
{
    // if (playbackQueue.size() < 2)
    // {
    //     LOG_WARN("Need at least 2 songs in queue to move");
    //     return;
    // }

    // size_t fromIndex = 0;
    // size_t toIndex = 0;

    // std::cout << "Enter from index (0-based): ";
    // if (!(std::cin >> fromIndex))
    // {
    //     std::cin.clear();
    //     std::cin.ignore(10000, '\n');
    //     LOG_WARN("Invalid input");
    //     return;
    // }

    // std::cout << "Enter to index (0-based): ";
    // if (!(std::cin >> toIndex))
    // {
    //     std::cin.clear();
    //     std::cin.ignore(10000, '\n');
    //     LOG_WARN("Invalid input");
    //     return;
    // }

    // playbackQueue.move(fromIndex, toIndex);
    LOG_WARN("Queue is not wired up in this debug build");
}

void DebugCLI::queueRemove()
{
    // if (playbackQueue.isEmpty())
    // {
    //     LOG_WARN("Queue is empty");
    //     return;
    // }

    // size_t index = 0;
    // std::cout << "Enter index to remove (0-based): ";
    // if (!(std::cin >> index))
    // {
    //     std::cin.clear();
    //     std::cin.ignore(10000, '\n');
    //     LOG_WARN("Invalid input");
    //     return;
    // }

    // playbackQueue.removeAt(index);
    LOG_WARN("Queue is not wired up in this debug build");
}
