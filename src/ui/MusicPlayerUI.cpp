#include "MusicPlayerUI.h"
#include "../data/LyricsManager.h"

#include <chrono>
#include <thread>
#include <utility>

#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>

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

void MusicPlayerUI::playCurrentSong()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist != nullptr)
    {
        Song *song = playlist->getCurrentSong();
        if (song != nullptr)
        {
            player.play(song->filePath);
            started = true;
            paused = false;
        }
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
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist != nullptr)
    {
        playlist->nextSong();
        Song *song = playlist->getCurrentSong();
        if (song != nullptr)
        {
            player.play(song->filePath);
            started = true;
            paused = false;
        }
    }
}

void MusicPlayerUI::playPrevious()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    if (playlist != nullptr)
    {
        playlist->previousSong();
        Song *song = playlist->getCurrentSong();
        if (song != nullptr)
        {
            player.play(song->filePath);
            started = true;
            paused = false;
        }
    }
}

ftxui::Element MusicPlayerUI::renderPlaylists()
{
    auto playlists = playlistManager.getPlaylists();

    std::vector<ftxui::Element> playlistElements;
    for (Playlist *playlistItem : playlists)
    {
        playlistElements.push_back(
            ftxui::text("  " + playlistItem->getName()));
    }

    return ftxui::window(
        ftxui::text("── Playlists ──") |
            ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 15) |
            ftxui::center,
        ftxui::vbox({
            ftxui::text("  "),
            ftxui::vbox(std::move(playlistElements)),
        }));
}
ftxui::Element MusicPlayerUI::renderSongs()
{
    std::vector<ftxui::Element> songElements;

    Playlist *playlist = playlistManager.getCurrentPlaylist();

    float currentTime = player.getCurrentTime();

    std::string currentLyric =
        lyricsManager.getCurrentLyric(currentTime);

    if (playlist != nullptr)
    {
        auto songList = playlist->getSongs();

        int index = 1;

        for (Song *song : songList)
        {
            songElements.push_back(
                ftxui::text(
                    " " + std::to_string(index) + ". " + song->title));

            index++;
        }
    }
    auto songsList = ftxui::vbox(std::move(songElements));

    auto lyricText =
        ftxui::paragraphAlignLeft(currentLyric.empty()
                                      ? " "
                                      : " " + currentLyric);

    auto lyricsBox = ftxui::window(
                         ftxui::text("── Lyrics ──"),
                         lyricText) |
                     ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3) |
                     ftxui::xflex;

    return ftxui::vbox({
        songsList,
        ftxui::filler(),
        lyricsBox,
    });
}
ftxui::Element MusicPlayerUI::renderProgressBar()
{
    float progress = playbackProgress.load();
    const int barWidth = 55;
    int filled = static_cast<int>(progress * barWidth);
    if (filled < 0)
        filled = 0;
    if (filled > barWidth)
        filled = barWidth;
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

// ftxui::Element MusicPlayerUI::renderLyrics()
// {
//     float currentTime = player.getCurrentTime();

//     std::string currentLyric =
//         lyricsManager.getCurrentLyric(currentTime);

//     return ftxui::window(
//         ftxui::text("── Lyrics ──"),
//         ftxui::vbox({
//             ftxui::text(" "),
//             ftxui::text(currentLyric) | ftxui::center,
//             ftxui::text(" "),
//         }));
// }

ftxui::Element MusicPlayerUI::renderNowPlaying()
{
    Playlist *playlist = playlistManager.getCurrentPlaylist();
    Song *currentSong =
        (playlist != nullptr) ? playlist->getCurrentSong() : nullptr;

    return ftxui::vbox({
        currentSong != nullptr
            ? ftxui::text(
                  "   NOW PLAYING: " +
                  currentSong->title +
                  " - " +
                  currentSong->artist)
            : ftxui::text("   Nothing playing"),
    });
}

void MusicPlayerUI::run()
{
    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    running = true;
    std::thread progressThread([this, &screen]
                               {
        while (running)
        {
            playbackProgress = player.getPlaybackProgress();
            screen.PostEvent(ftxui::Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } });

    ftxui::ButtonOption option;
    option.transform = [](const ftxui::EntryState &state)
    {
        return ftxui::text(state.label) | ftxui::center | ftxui::borderRounded;
    };

    ftxui::ButtonOption playOption;
    playOption.transform = [this](const ftxui::EntryState &state)
    {
        std::string label;
        if (!started || paused)
            label = "▶";
        else
            label = "⏯";

        return ftxui::text(label) | ftxui::center | ftxui::borderRounded;
    };

    auto previousButton = ftxui::Button("⏮", [this]
                                        { playPrevious(); }, option);
    auto playButton = ftxui::Button("", [this]
                                    { togglePlayPause(); }, playOption);
    auto nextButton = ftxui::Button("⏭", [this]
                                    { playNext(); }, option);

    auto component = ftxui::Container::Horizontal({
        previousButton,
        playButton,
        nextButton,
    });

    auto renderer = ftxui::Renderer(component, [this, previousButton, playButton, nextButton]
                                    {
        auto playlist_panel = renderPlaylists();
        auto songs_panel =
    ftxui::window(
        ftxui::text("── Songs ──"),
        renderSongs()
    ) |
    ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 60);
        // auto lyrics_panel = renderLyrics();
        auto progressBar = renderProgressBar();
        auto nowPlaying = renderNowPlaying();

        auto controls = ftxui::hbox({
            previousButton->Render()
                | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 10)
                | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3),

            playButton->Render()
                | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 11)
                | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3),

            nextButton->Render()
                | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 10)
                | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3),

            ftxui::vbox({
                nowPlaying,
                ftxui::text(" "),
                progressBar,
            }) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3),
        });

       auto mainArea = ftxui::hbox({
    playlist_panel | ftxui::flex_grow,
    songs_panel,
}) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 30);

        auto document = ftxui::vbox({
    mainArea | ftxui::flex,
    controls,
}) |
            ftxui::borderRounded |
            ftxui::bgcolor(ftxui::Color::RGB(0, 0, 60)) |
            ftxui::color(ftxui::Color::CyanLight);

        return document |
            ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 100); });

    screen.Loop(renderer);

    running = false;
    if (progressThread.joinable())
    {
        progressThread.join();
    }

    player.stop();
}
