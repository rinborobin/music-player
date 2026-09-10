#ifndef LYRICS_MANAGER_H
#define LYRICS_MANAGER_H

#include <string>
#include <vector>

struct LyricLine
{
    float timestamp;
    std::string text;
};

class LyricsManager
{
public:
    bool loadLyrics(const std::string &filePath);

    std::string getCurrentLyric(float currentTime);

    std::vector<LyricLine> getLyrics();

    void clear();

private:
    std::vector<LyricLine> lyrics;
};

#endif