#include <iostream>
#include "Playlist.h"

Playlist::Playlist()
{
    head = nullptr;
    tail = nullptr;
    current = nullptr;
}

void Playlist::addSong(const std::string &title,
                       const std::string &artist,
                       const std::string &filePath)
{
    Song *newSong = new Song;

    newSong->title = title;
    newSong->artist = artist;
    newSong->filePath = filePath;

    newSong->next = nullptr;
    newSong->prev = nullptr;

    if (head == nullptr)
    {
        head = newSong;
        tail = newSong;
        current = newSong;
    }
    else
    {
        newSong->prev = tail;
        tail->next = newSong;
        tail = newSong;
    }
}

void Playlist::displayPlaylist()
{
    Song *current = head;

    while (current != nullptr)
    {
        std::cout << "Title: " << current->title << std::endl;
        std::cout << "Artist: " << current->artist << std::endl
                  << std::endl;
        current = current->next;
    }
}