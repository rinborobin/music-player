#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include "../audio/MusicEngine.h"
#include "../data/PlaylistManager.h"
#include "../data/LyricsManager.h"
#include "../data/Queue.h"

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

    MusicEngine &player;
    PlaylistManager &playlistManager;
    LyricsManager &lyricsManager;

    void run();

    void playCurrentSong();
    void playSong(Song *song);
    void togglePlayPause();
    void playNext();
    void playPrevious();
    void addSelectedSongToQueue();

private:
    // --- Playback / state helpers ---
    Playlist *getCurrentPlaylist() const;
    Song *getCurrentSong() const;

    void syncSelectedSongWithCurrent();
    void resetPlaybackState();
    void refreshSongNames();

    // --- Component builders ---
    ftxui::Component createSongMenu(ftxui::ScreenInteractive &screen);
    ftxui::Component createPlaylistMenu();
    ftxui::Component createControlButtons();

    ftxui::Element renderMainArea(
        ftxui::Component playlistMenu,
        ftxui::Component songMenu) const;
    ftxui::Element renderControlBar(
        ftxui::Component controlButtons) const;

    // --- Renderers ---
    ftxui::Element renderProgressBar() const;
    ftxui::Element renderNowPlaying() const;
    ftxui::Element renderQueue() const;

    // --- State ---
    bool started = false;
    bool paused = false;
    bool playPending_ = false;

    int selectedSong = 0;
    int selectedPlaylist_ = 0;
    Song *playingSong = nullptr;
    int playingSongIndex = -1;

    std::vector<std::string> songNames_;
    std::vector<std::string> playlistNames_;

    Queue playbackQueue;

    std::atomic<float> playbackProgress{0.0f};
    std::atomic<bool> running{false};
};

#endif // MUSIC_PLAYER_UI_H
