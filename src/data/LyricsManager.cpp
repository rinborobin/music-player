#include "LyricsManager.h"

#include <fstream>
#include <sstream>

bool LyricsManager::loadLyrics(const std::string &filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        return false;
    }

    lyrics.clear();

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        size_t openBracket = line.find('[');
        size_t closeBracket = line.find(']');

        if (openBracket == std::string::npos ||
            closeBracket == std::string::npos)
        {
            continue;
        }

        std::string timestampText =
            line.substr(openBracket + 1,
                        closeBracket - openBracket - 1);

        std::string lyricText =
            line.substr(closeBracket + 1);

        if (lyricText.empty())
        {
            continue;
        }

        size_t colon = timestampText.find(':');

        if (colon == std::string::npos)
        {
            continue;
        }

        float minutes =
            std::stof(timestampText.substr(0, colon));

        float seconds =
            std::stof(timestampText.substr(colon + 1));

        float timestamp =
            minutes * 60.0f + seconds;

        LyricLine lyric;
        lyric.timestamp = timestamp;
        lyric.text = lyricText;

        lyrics.push_back(lyric);
    }

    file.close();

    return true;
}

std::vector<LyricLine> LyricsManager::getLyrics()
{
    return lyrics;
}

void LyricsManager::clear()
{
    lyrics.clear();
}

std::string LyricsManager::getCurrentLyric(float currentTime)
{
    if (lyrics.empty())
        return "";

    std::string currentLyric = "";

    for (const LyricLine &line : lyrics)
    {
        if (line.timestamp <= currentTime)
        {
            currentLyric = line.text;
        }
        else
        {
            break;
        }
    }

    return currentLyric;
}