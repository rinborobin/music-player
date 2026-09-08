#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "Song.h"
#include <vector>

class Playlist
{
private:
    std::string name;
    Song *head;
    Song *tail;
    Song *current;

public:
    Playlist(const std::string &name);

    void addSong(const std::string &title,
                 const std::string &artist,
                 const std::string &filePath);
    Song *getCurrentSong();
    Song *searchSongTitle(const std::string &title);
    Song *searchSongArtist(const std::string &artist);
    void removeSong(Song *song);
    std::vector<Song *> getSongs();
    void displayPlaylist();
    void nextSong();
    void previousSong();
    void displayCurrentSong();

    std::string getName();
};

#endif