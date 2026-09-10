#include "audio/MusicEngine.h"
#include "data/Playlist.h"
#include "data/PlaylistManager.h"
#include "ui/MusicPlayerUI.h"
#include "data/LyricsManager.h"

#include <iostream>

int main()
{
    PlaylistManager playlistManager;
    MusicEngine player;

    LyricsManager lyricsManager;

    playlistManager.createPlaylist("My Playlist");
    playlistManager.createPlaylist("My Playlist");
    playlistManager.createPlaylist("Favorites");
    playlistManager.createPlaylist("Chill");

    Playlist *playlist = playlistManager.getCurrentPlaylist();

    if (playlist != nullptr)
    {
        // playlist->addSong("Merry Christmas, i miss you", "Alex Chricton", "../music/Alex Crichton - Merry Christmas, i miss you (Lyrics).mp3");
        playlist->addSong(
            "What If I Call",
            "Alex Crichton",
            "../music/Alex Crichton - What If I Call (Lyrics).mp3",
            "../music/Alex Crichton - What If I Call.lrc");

        bool loaded = lyricsManager.loadLyrics(
            "../music/Alex Crichton - What If I Call.lrc");

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