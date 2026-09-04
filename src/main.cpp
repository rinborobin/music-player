

#include "data/Playlist.h"
#include "audio/MusicEngine.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <ftxui/screen/screen.hpp> // for Full, Screen
#include <memory>                  // for allocator

#include "ftxui/dom/node.hpp"     // for Render
#include "ftxui/screen/color.hpp" // for ftxui

using namespace ftxui;

int main()
{
    bool started = false;
    bool paused = false;

    Playlist playlist;
    MusicEngine player;

    std::atomic<float> playbackProgress{0.0f};
    std::atomic<bool> running{true};

    playlist.addSong("Merry Christmas, i miss you", "Alex Chricton", "../music/Alex Crichton - Merry Christmas, i miss you (Lyrics).mp3");
    playlist.addSong("What If I Call", "Alex Chricton", "../music/Alex Crichton - What If I Call (Lyrics).mp3");

    std::cout
        << "Before getSongs()" << std::endl;

    auto songList = playlist.getSongs();

    std::cout << "After getSongs()" << std::endl;

    std::cout << "Number of songs: " << songList.size() << std::endl;

    std::vector<Element> songElements;

    int index = 1;

    for (Song *song : songList)
    {
        songElements.push_back(
            text(" " + std::to_string(index) + ". " + song->title));

        index++;
    }

    auto songListElement = vbox({songElements});

    Song *currentSong = playlist.getCurrentSong();

    ButtonOption option;

    ButtonOption playOption;

    playOption.transform = [&](const EntryState &state)
    {
        std::string label;

        if (!started || paused)
            label = "▶";
        else
            label = "⏯";

        return text(label) | center | borderRounded;
    };

    option.transform = [](const EntryState &state)
    {
        return text(state.label) | center | borderRounded;
    };

    ScreenInteractive screen = ScreenInteractive::TerminalOutput();

    std::thread progressThread([&]
                               {
        while (running)
        {
            playbackProgress = player.getPlaybackProgress();
            screen.PostEvent(Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } });

    auto previousButton = Button("⏮", [&]
                                 {  playlist.previousSong();

                                    Song* song = playlist.getCurrentSong();

                                    if (song != nullptr)
                                    {
                                        player.play(song->filePath);
                                            started = true;
                                    } }, option);

    auto playButton = Button("", [&]
                             {
                                 if (!started)
                                 {
                                     Song *song = playlist.getCurrentSong();

                                     if (song != nullptr)
                                     {
                                         player.play(song->filePath);
                                         started = true;
                                         paused = false;
                                     }
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
                                 } }, playOption);

    auto nextButton = Button("⏭", [&]
                             {  playlist.nextSong();

                                Song* song = playlist.getCurrentSong();

                                if (song != nullptr)
                                {
                                    player.play(song->filePath);
                                        started = true;
                                } }, option);

    auto controls = hbox({
        previousButton->Render() | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 3),

        playButton->Render() | size(WIDTH, EQUAL, 11) | size(HEIGHT, EQUAL, 3),

        nextButton->Render() | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 3),
    });

    auto component = Container::Horizontal({
        previousButton,
        playButton,
        nextButton,
    });

    auto playlist_panel = window(
        text("── Albums ──") | size(WIDTH, GREATER_THAN, 15) | center,
        vbox({
            text("  "),

            text(" > Album 1"),
            text("  Album 2"),
            text("  Next Album 3"),
            text("  The Other New Album 4"),
        }));

    auto songs = window(
        text("── Songs ──"),
        vbox({text("                                          "),
              songListElement}));

    auto mainArea = hbox({
        playlist_panel | flex_grow_factor(4),
        songs | flex_grow_factor(6),
    });

    auto document = vbox({
                        mainArea | flex,
                        controls,
                    }) |
                    borderRounded | bgcolor(Color::RGB(0, 0, 60)) | color(Color::CyanLight);

    auto renderer = Renderer(component, [&]
                             {

        Song *currentSong = playlist.getCurrentSong();

        float progress = playbackProgress.load();
        const int barWidth = 55;
        int filled = static_cast<int>(progress * barWidth);
        int empty = barWidth - filled;

        const char *filledChar = u8"\u2501";
        const char *emptyChar = u8"\u2500";

        std::string filledPart;
        for (int i = 0; i < filled; ++i)
            filledPart += filledChar;

        std::string emptyPart;
        for (int i = 0; i < empty; ++i)
            emptyPart += emptyChar;

        auto progressBar = hbox({
            text("   "),
            text(filledPart) | color(Color::CyanLight),
            text(emptyPart) | color(Color::RGB(0,51,102)),
        });

        auto nowPlaying = vbox({

        currentSong != nullptr

            ? 
            text("   NOW PLAYING: "+currentSong->title + " - " + currentSong->artist)
            : text("   Nothing playing"),
        
    });

    auto controls = hbox({
        previousButton->Render()
            | size(WIDTH, EQUAL, 10)
            | size(HEIGHT, EQUAL, 3),

        playButton->Render()
            | size(WIDTH, EQUAL, 11)
            | size(HEIGHT, EQUAL, 3),

        nextButton->Render()
            | size(WIDTH, EQUAL, 10)
            | size(HEIGHT, EQUAL, 3),

             vbox({
        nowPlaying,
        text(" "),
        progressBar,
    }) | flex,
    });

    auto document = vbox({
        mainArea | flex,
        controls,
    }) |
        borderRounded |
        bgcolor(Color::RGB(0, 0, 60)) |
        color(Color::CyanLight);

    return document |
        size(WIDTH, EQUAL, 100) |
        size(HEIGHT, EQUAL, 40); });

    screen.Loop(renderer);

    running = false;
    progressThread.join();

    player.stop();

    return 0;
}