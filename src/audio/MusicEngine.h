#ifndef MUSIC_ENGINE_H
#define MUSIC_ENGINE_H

#include <string>
#include "miniaudio.h"

class MusicEngine
{
public:
    MusicEngine();
    ~MusicEngine();

    bool play(const std::string &filePath);
    bool isPlaying();
    void stop();
    void pause();
    void resume();
    void next(Song *song);

private:
    ma_engine engine;
    ma_sound sound;

    bool initialized;
    bool soundInitialized;
};

#endif