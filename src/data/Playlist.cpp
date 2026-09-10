#include <iostream>
#include "Playlist.h"

Playlist::Playlist(const std::string &name)
{
    this->name = name;

    head = nullptr;
    tail = nullptr;
    current = nullptr;
}

std::string Playlist::getName()
{
    return name;
}

void Playlist::addSong(const std::string &title,
                       const std::string &artist,
                       const std::string &filePath,
                       const std::string &lyricPath)
{
    Song *newSong = new Song;

    newSong->title = title;
    newSong->artist = artist;
    newSong->filePath = filePath;
    newSong->lyricPath = lyricPath;

    if (head == nullptr)
    {
        head = newSong;
        tail = newSong;
        current = newSong;

        newSong->next = newSong;
        newSong->prev = newSong;
    }
    else
    {
        newSong->prev = tail;
        newSong->next = head;

        tail->next = newSong;
        head->prev = newSong;

        tail = newSong;
    }
}
Song *Playlist::searchSongTitle(const std::string &title)
{

    Song *currentSong = head;

    while (currentSong != nullptr)
    {
        if (currentSong->title == title)
        {
            return currentSong;
        }
        currentSong = currentSong->next;
    }

    return nullptr;
}

Song *Playlist::searchSongArtist(const std::string &artist)
{

    Song *currentSong = head;

    while (currentSong != nullptr)
    {
        if (currentSong->artist == artist)
        {
            return currentSong;
        }
        currentSong = currentSong->next;
    }

    return nullptr;
}

void Playlist::removeSong(Song *song)
{
    if (song == nullptr)
        return;

    if (song->prev != nullptr)
    {
        song->prev->next = song->next;
    }
    else
    {
        head = song->next;
    }

    if (song->next != nullptr)
    {
        song->next->prev = song->prev;
    }
    else
    {
        tail = song->prev;
    }

    if (current == song)
    {
        current = song->next;

        if (current == nullptr)
        {
            current = tail;
        }
    }

    std::cout << song->title << " - "
              << song->artist
              << " Removed from the playlist."
              << std::endl;

    delete song;
}

Song *Playlist::getCurrentSong()
{
    return current;
}

void Playlist::displayPlaylist()
{
    if (head == nullptr)
        return;

    Song *currentSong = head;
    int index = 1;

    do
    {
        std::cout << index << ". "
                  << currentSong->title << " - "
                  << currentSong->artist << std::endl
                  << std::endl;

        currentSong = currentSong->next;
        index++;

    } while (currentSong != head);
}

std::vector<Song *> Playlist::getSongs()
{
    std::vector<Song *> songs;

    if (head == nullptr)
        return songs;

    Song *currentSong = head;

    do
    {
        songs.push_back(currentSong);
        currentSong = currentSong->next;

    } while (currentSong != head);

    return songs;
}

void Playlist::nextSong()
{
    if (current == nullptr)
        return;

    current = current->next;
}
void Playlist::previousSong()
{
    if (current == nullptr)
        return;

    current = current->prev;
}
void Playlist::displayCurrentSong()
{
    std::cout << "Now playing: " + current->title + " - " + current->artist << std::endl;
}