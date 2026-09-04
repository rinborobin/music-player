#include <iostream>

#include "data/Playlist.h"
#include "audio/MusicEngine.h"

#include <iostream>

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

            text(" > Test 1"),
            text("  Test 2"),
            text("  Test 3"),
            text("  Test 4"),
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
                                    
        auto nowPlaying = vbox({

        currentSong != nullptr
            ? text("   NOW PLAYING: "+currentSong->title + " - " + currentSong->artist)
            : text("  Nothing playing")
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

        nowPlaying
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

    player.stop();

    return 0;
}