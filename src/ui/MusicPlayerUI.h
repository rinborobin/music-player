#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include "../audio/MusicEngine.h"
#include "../data/PlaylistManager.h"
#include "../data/LyricsManager.h"

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

class MusicPlayerUI
{
public:
    MusicPlayerUI(
        MusicEngine &engine,
        PlaylistManager &playlistManager,
        LyricsManager &lyricsManager);
    ~MusicPlayerUI();

    void run();

private:
    void playCurrentSong();
    void togglePlayPause();
    void playNext();
    void playPrevious();

    ftxui::Element renderPlaylists();
    ftxui::Element renderSongs();
    // ftxui::Element renderLyrics();
    ftxui::Element renderProgressBar();
    ftxui::Element renderNowPlaying();

    MusicEngine &player;
    PlaylistManager &playlistManager;
    LyricsManager &lyricsManager;

    bool started = false;
    bool paused = false;

    std::atomic<float> playbackProgress{0.0f};
    std::atomic<bool> running{false};
};

#endif // MUSIC_PLAYER_UI_H
