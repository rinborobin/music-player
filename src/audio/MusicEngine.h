#ifndef MUSIC_ENGINE_H
#define MUSIC_ENGINE_H

#include <atomic>
#include <string>
#include "miniaudio.h"

struct Song;

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

    float getCurrentTime();
    float getDuration();
    float getPlaybackProgress();

    bool consumeFinished();

private:
    static void soundEndCallback(void *userData, ma_sound *sound);

    ma_engine engine;
    ma_sound sound;

    bool initialized;
    bool soundInitialized;
    std::atomic<bool> finished_{false};
};

#endif