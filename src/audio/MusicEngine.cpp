#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "../data/Playlist.h"
#include "MusicEngine.h"

#include <iostream>

MusicEngine::MusicEngine()
{
    initialized = (ma_engine_init(nullptr, &engine) == MA_SUCCESS);
    soundInitialized = false;
}

MusicEngine::~MusicEngine()
{
    if (soundInitialized)
    {
        ma_sound_uninit(&sound);
    }

    if (initialized)
    {
        ma_engine_uninit(&engine);
    }
}

bool MusicEngine::isPlaying()
{
    return ma_sound_is_playing(&sound);
}

float MusicEngine::getCurrentTime()
{
    if (!soundInitialized)
        return 0.0f;

    float cursor;
    ma_sound_get_cursor_in_seconds(&sound, &cursor);

    return cursor;
}

float MusicEngine::getDuration()
{
    if (!soundInitialized)
        return 0.0f;

    float length;
    ma_sound_get_length_in_seconds(&sound, &length);

    return length;
}

float MusicEngine::getPlaybackProgress()
{
    if (!soundInitialized)
        return 0.0f;

    float length = 0.0f;
    ma_sound_get_length_in_seconds(&sound, &length);

    if (length == 0.0f)
        return 0.0f;

    float cursor = 0.0f;
    ma_sound_get_cursor_in_seconds(&sound, &cursor);

    return cursor / length;
}

bool MusicEngine::play(const std::string &filePath)
{
    if (!initialized)
    {
        return false;
    }

    if (soundInitialized)
    {
        ma_sound_uninit(&sound);
        soundInitialized = false;
    }

    ma_result result = ma_sound_init_from_file(
        &engine,
        filePath.c_str(),
        MA_SOUND_FLAG_STREAM,
        nullptr,
        nullptr,
        &sound);

    if (result != MA_SUCCESS)
    {
        return false;
    }

    soundInitialized = true;

    return ma_sound_start(&sound) == MA_SUCCESS;
}

void MusicEngine::stop()
{
    if (soundInitialized)
    {
        ma_sound_stop(&sound);
    }
}

void MusicEngine::pause()
{
    if (soundInitialized)
    {
        ma_sound_stop(&sound);
    }
}

void MusicEngine::resume()
{
    if (soundInitialized)
    {
        ma_sound_start(&sound);
    }
}

void MusicEngine::next(Song *song)
{
    if (song != nullptr)
    {
        std::cout << "Now playing: "
                  << song->title << " - "
                  << song->artist << std::endl;

        play(song->filePath);
    }
    else
        std::cout << "There is no next song in the playlist." << std::endl;
}
