#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "Song.h"

class Playlist
{
private:
    Song *head;
    Song *tail;
    Song *current;

public:
    Playlist();

    void addSong(const std::string &title,
                 const std::string &artist,
                 const std::string &filePath);
    void displayPlaylist();
};

#endif