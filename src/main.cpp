#include <iostream>
#include "Playlist.h"

int main()
{
    Playlist playlist;

    Playlist *head = nullptr;

    playlist.addSong(
        "Blinding Lights",
        "The Weeknd",
        "music/blinding_lights.mp3");

    playlist.addSong(
        "Starboy",
        "The Weeknd",
        "music/starboy.mp3");

    playlist.addSong(
        "Save Your Tears",
        "The Weeknd",
        "music/save_your_tears.mp3");

    std::cout << "Songs added!\n";

    playlist.displayPlaylist();

    return 0;
}