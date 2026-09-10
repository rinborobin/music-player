#ifndef SONG_H
#define SONG_H

#include <string>

struct Song
{
    std::string title;
    std::string artist;
    std::string filePath;
    std::string lyricPath;

    Song *next;
    Song *prev;
};

#endif