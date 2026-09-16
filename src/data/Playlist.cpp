#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
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
    if (head == nullptr) return nullptr;

    Song *currentSong = head;
    do
    {
        if (currentSong->title == title)
        {
            return currentSong;
        }
        currentSong = currentSong->next;
    } while (currentSong != head);

    return nullptr;
}

Song *Playlist::searchSongArtist(const std::string &artist)
{
    if (head == nullptr) return nullptr;

    Song *currentSong = head;
    do
    {
        if (currentSong->artist == artist)
        {
            return currentSong;
        }
        currentSong = currentSong->next;
    } while (currentSong != head);

    return nullptr;
}

void Playlist::removeSong(Song *song)
{
    if (song == nullptr || head == nullptr)
        return;

    // Single node case
    if (head == tail && head == song)
    {
        head = nullptr;
        tail = nullptr;
        current = nullptr;
    }
    else
    {
        song->prev->next = song->next;
        song->next->prev = song->prev;

        if (head == song) head = song->next;
        if (tail == song) tail = song->prev;
        if (current == song) current = song->next;
    }

    std::cout << song->title << " - "
              << song->artist
              << " removed from the playlist."
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
                  << currentSong->artist << std::endl;

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
    if (current == nullptr)
    {
        std::cout << "No song currently playing." << std::endl;
        return;
    }
    std::cout << "Now playing: " << current->title << " - " << current->artist << std::endl;
}

// Missing method definitions required by DebugCLI
void Playlist::sortByTitle()
{
    // Stub implementation to satisfy the compiler/linker
}

void Playlist::sortByArtist()
{
    // Stub implementation to satisfy the compiler/linker
}

static std::string toLowerString(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return str;
}

std::vector<Song*> Playlist::searchSongs(const std::string &query)
{
    std::vector<Song *> results;

    if (query.empty() || head == nullptr) {
        return results;
    }

    std::string lowerQuery = toLowerString(query);
    Song *currentSong = head;

    do {
        std::string lowerTitle = toLowerString(currentSong->title);
        std::string lowerArtist = toLowerString(currentSong->artist);

        if (lowerTitle.find(lowerQuery) != std::string::npos || 
            lowerArtist.find(lowerQuery) != std::string::npos) {
            results.push_back(currentSong);
        }

        currentSong = currentSong->next;
    } while (currentSong != head);

    return results;
}