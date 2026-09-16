#include "MusicPlayerUI.h"
#include "../data/LyricsManager.h"
#include "../data/Song.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>

#undef max
#undef min

MusicPlayerUI::MusicPlayerUI(
    MusicEngine &engine,
    PlaylistManager &playlistManager,
    LyricsManager &lyricsManager)
    : player(engine),
      playlistManager(playlistManager),
      lyricsManager(lyricsManager)
{
}

MusicPlayerUI::~MusicPlayerUI()
{
    running = false;
    player.stop();
}

// ---------------------------------------------------------------------------
// Playback / state helpers
// ---------------------------------------------------------------------------

Playlist *MusicPlayerUI::getCurrentPlaylist() const
{
    return playlistManager.getCurrentPlaylist();
}

Song *MusicPlayerUI::getCurrentSong() const
{
    Playlist *playlist = getCurrentPlaylist();
    return (playlist != nullptr) ? playlist->getCurrentSong() : nullptr;
}

void MusicPlayerUI::syncSelectedSongWithCurrent()
{
    Playlist *playlist = getCurrentPlaylist();
    if (playlist == nullptr)
        return;

    Song *currentSong = playlist->getCurrentSong();
    const auto &songs = playlist->getSongs();

    for (int i = 0; i < static_cast<int>(songs.size()); ++i)
    {
        if (songs[i] == currentSong)
        {
            selectedSong = i;
            return;
        }
    }
}

void MusicPlayerUI::resetPlaybackState()
{
    player.stop();
    started = false;
    paused = false;
    lyricsManager.clear();
}

void MusicPlayerUI::refreshSongNames()
{
    songNames_.clear();

    Playlist *playlist = playlistManager.getCurrentPlaylist();

    if (playlist == nullptr)
        return;

    auto songs = playlist->getSongs();

    for (Song *song : songs)
    {
        songNames_.push_back(song->title);
    }
}
// ---------------------------------------------------------------------------
// Playback controls
// ---------------------------------------------------------------------------

void MusicPlayerUI::playCurrentSong()
{
    Song *song = getCurrentSong();

    if (song == nullptr)
    {
        LOG_WARN("No current song to play");
        return;
    }

    LOG_INFO("Attempting to play: " + song->title + " (" + song->filePath + ")");

    lyricsManager.clear();
    lyricsManager.loadLyrics(song->lyricPath);

    if (player.play(song->filePath))
    {
        LOG_INFO("Playback started: " + song->title);
        playingSong = song;
        playingSongIndex = selectedSong;
        started = true;
        paused = false;
    }
    else
    {
        LOG_ERROR("Failed to play: " + song->filePath);
        playingSong = nullptr;
        playingSongIndex = -1;
        started = false;
    }
}

void MusicPlayerUI::togglePlayPause()
{
    if (!started)
    {
        playCurrentSong();
    }
    else if (player.isPlaying())
    {
        player.pause();
        paused = true;
    }
    else
    {
        player.resume();
        paused = false;
    }
}

void MusicPlayerUI::playNext()
{
    Playlist *playlist = getCurrentPlaylist();
    if (playlist == nullptr)
        return;

    playlist->nextSong();
    syncSelectedSongWithCurrent();
    playCurrentSong();
}

void MusicPlayerUI::playPrevious()
{
    Playlist *playlist = getCurrentPlaylist();
    if (playlist == nullptr)
        return;

    playlist->previousSong();
    syncSelectedSongWithCurrent();
    playCurrentSong();
}

// ---------------------------------------------------------------------------
// Renderers
// ---------------------------------------------------------------------------

ftxui::Element MusicPlayerUI::renderProgressBar() const
{
    float progress = playbackProgress.load();
    const int barWidth = 55;
    int filled = static_cast<int>(progress * barWidth);
    filled = std::max(0, std::min(filled, barWidth));
    int empty = barWidth - filled;

    const char *filledChar = u8"\u2501";
    const char *emptyChar = u8"\u2500";

    std::string filledPart;
    for (int i = 0; i < filled; ++i)
        filledPart += filledChar;

    std::string emptyPart;
    for (int i = 0; i < empty; ++i)
        emptyPart += emptyChar;

    return ftxui::hbox({
        ftxui::text("   "),
        ftxui::text(filledPart) | ftxui::color(ftxui::Color::RGB(255, 0, 0)),
        ftxui::text(emptyPart) | ftxui::color(ftxui::Color::RGB(0, 51, 102)),
    });
}

ftxui::Element MusicPlayerUI::renderNowPlaying() const
{
    if (playingSong == nullptr)
    {
        return ftxui::text("   Nothing playing");
    }

    return ftxui::text(
        "   NOW PLAYING: " +
        playingSong->title +
        " - " +
        playingSong->artist);
}

// ---------------------------------------------------------------------------
// Component builders
// ---------------------------------------------------------------------------

ftxui::Component MusicPlayerUI::createSongMenu(ftxui::ScreenInteractive &screen)
{
    refreshSongNames();

    ftxui::MenuOption songOption;

    songOption.entries_option.transform =
        [this](const ftxui::EntryState &state)
    {
        Playlist *playlist = playlistManager.getCurrentPlaylist();

        if (playlist == nullptr)
            return ftxui::text("");

        auto songs = playlist->getSongs();

        if (state.index < 0 ||
            state.index >= static_cast<int>(songs.size()))
        {
            return ftxui::text("");
        }

        Song *song = songs[state.index];

        std::string prefix =
            (song == playingSong) ? "> " : "  ";

        auto element =
            ftxui::text(prefix + song->title);

        if (state.focused)
        {
            return element | ftxui::bgcolor(ftxui::Color::RGB(240, 150, 90)) | ftxui::color(ftxui::Color::White) | ftxui::bold;
        }

        return element | ftxui::color(ftxui::Color::White);
    };

    songOption.on_change = [&]
    {
        if (playlistManager.getCurrentPlaylist() == nullptr)
            return;

        playPending_ = true;
        screen.PostEvent(ftxui::Event::Custom);
    };

    auto songMenu = ftxui::Menu(&songNames_, &selectedSong, songOption);

    return songMenu | ftxui::CatchEvent([&](ftxui::Event event)
                                        {
        if (event.is_mouse() &&
            event.mouse().button == ftxui::Mouse::Left &&
            event.mouse().motion == ftxui::Mouse::Pressed)
        {
            playPending_ = true;
            screen.PostEvent(ftxui::Event::Custom);
        }

        return false; });
}

ftxui::Component MusicPlayerUI::createPlaylistMenu()
{
    playlistNames_.clear();

    for (Playlist *playlist : playlistManager.getPlaylists())
    {
        playlistNames_.push_back(playlist->getName());
    }

    ftxui::MenuOption playlistOption;

    playlistOption.on_change = [&]
    {
        playlistManager.selectPlaylist(selectedPlaylist_);

        selectedSong = -1;
        refreshSongNames();
    };

    return ftxui::Menu(
        &playlistNames_,
        &selectedPlaylist_,
        playlistOption);
}

ftxui::Component MusicPlayerUI::createControlButtons()
{
    ftxui::ButtonOption option;
    option.transform = [](const ftxui::EntryState &state)
    {
        return ftxui::text(state.label) |
               ftxui::center |
               ftxui::borderRounded |
               ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 10) |
               ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3);
    };

    ftxui::ButtonOption playOption;
    playOption.transform = [this](const ftxui::EntryState &state)
    {
        std::string label = (!started || paused) ? "▶" : "⏯";
        return ftxui::text(label) |
               ftxui::center |
               ftxui::borderRounded |
               ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 11) |
               ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3);
    };

    auto previousButton = ftxui::Button(
        "⏮",
        [this]
        { playPrevious(); },
        option);

    auto playButton = ftxui::Button(
        "",
        [this]
        { togglePlayPause(); },
        playOption);

    auto nextButton = ftxui::Button(
        "⏭",
        [this]
        { playNext(); },
        option);

    return ftxui::Container::Horizontal({
        previousButton,
        playButton,
        nextButton,
    });
}

// ---------------------------------------------------------------------------
// Layout renderers
// ---------------------------------------------------------------------------

ftxui::Element MusicPlayerUI::renderMainArea(
    ftxui::Component playlistMenu,
    ftxui::Component songMenu) const
{
    auto playlistPanel = ftxui::window(
                             ftxui::text("── Playlists ──") |
                                 ftxui::color(ftxui::Color::RGB(200, 0, 0)) |
                                 ftxui::bold,
                             playlistMenu->Render() |
                                 ftxui::color(ftxui::Color::White)) |
                         ftxui::color(ftxui::Color::RGB(200, 0, 0));

    auto songsPanel = ftxui::window(
                          ftxui::text("── Songs ──"),
                          songNames_.empty()
                              ? ftxui::text("  No songs in this playlist")
                              : songMenu->Render()) |
                      ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 60);

    return ftxui::hbox({
               playlistPanel | ftxui::flex_grow,
               songsPanel,
           }) |
           ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 30);
}

ftxui::Element MusicPlayerUI::renderControlBar(
    ftxui::Component controlButtons) const
{
    auto progressBar = renderProgressBar();
    auto nowPlaying = renderNowPlaying();

    auto infoBox = ftxui::vbox({
                       nowPlaying,
                       ftxui::text(" "),
                       progressBar,
                   }) |
                   ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3);

    return ftxui::hbox({
        controlButtons->Render() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3),
        infoBox,
    });
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void MusicPlayerUI::run()
{
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    running = true;
    std::thread progressThread([this, &screen]
                               {
        while (running)
        {
            playbackProgress = player.getPlaybackProgress();
            screen.PostEvent(ftxui::Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } });

    auto songMenu = createSongMenu(screen);
    auto playlistMenu = createPlaylistMenu();
    auto controlButtons = createControlButtons();

    auto mainContainer = ftxui::Container::Horizontal({
        playlistMenu,
        songMenu,
    });

    auto component = ftxui::Container::Vertical({
        mainContainer,
        controlButtons,
    });

    component = component | ftxui::CatchEvent([&](ftxui::Event event)
                                              {
        if (event == ftxui::Event::Custom && playPending_)
        {
            playPending_ = false;

            Playlist *playlist = getCurrentPlaylist();
            if (playlist != nullptr &&
                selectedSong >= 0 &&
                selectedSong < static_cast<int>(playlist->getSongs().size()))
            {
                playlist->selectSong(selectedSong);
                playCurrentSong();
            }

            return true;
        }

        return false; });

    auto renderer = ftxui::Renderer(
        component,
        [this, playlistMenu, songMenu, controlButtons]
        {
            auto mainArea = renderMainArea(playlistMenu, songMenu);
            auto controls = renderControlBar(controlButtons);

            auto document = ftxui::vbox({
                                  mainArea | ftxui::flex,
                                  controls,
                              }) |
                              ftxui::borderRounded |
                              ftxui::bgcolor(ftxui::Color::RGB(0, 0, 60)) |
                              ftxui::color(ftxui::Color::CyanLight);

            return document |
                   ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 100) |
                   ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 35) |
                   ftxui::center;
        });

    screen.Loop(renderer);

    running = false;
    if (progressThread.joinable())
    {
        progressThread.join();
    }

    player.stop();
}
