#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "../data/Playlist.h"
#include "../utils/Logger.h"
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
        LOG_ERROR("MusicEngine is not initialized");
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
        LOG_ERROR("Failed to load audio file: " + filePath);
        return false;
    }

    soundInitialized = true;

    bool started = (ma_sound_start(&sound) == MA_SUCCESS);
    if (started)
    {
        LOG_INFO("Started playback: " + filePath);
    }
    else
    {
        LOG_ERROR("Failed to start playback: " + filePath);
    }

    return started;
}

void MusicEngine::stop()
{
    if (soundInitialized)
    {
        LOG_INFO("Stopping playback");
        ma_sound_stop(&sound);
    }
}

void MusicEngine::pause()
{
    if (soundInitialized)
    {
        LOG_INFO("Pausing playback");
        ma_sound_stop(&sound);
    }
}

void MusicEngine::resume()
{
    if (soundInitialized)
    {
        LOG_INFO("Resuming playback");
        ma_sound_start(&sound);
    }
}

void MusicEngine::next(Song *song)
{
    if (song != nullptr)
    {
        LOG_INFO("Now playing: " + song->title + " - " + song->artist);
        play(song->filePath);
    }
    else
    {
        LOG_WARN("There is no next song in the playlist.");
    }
}
