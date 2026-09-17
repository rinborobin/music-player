/*
Add Song to Queue
Clear Queue
Play Next Queue
*/

#include "Queue.h"
#include "Song.h"

#include <iostream>

bool Queue::isEmpty()
{
    return frontSong == nullptr;
}

int Queue::getQueueSize()
{
    return count;
}

void Queue::addSongToQueue(Song *song)
{
    if (song == nullptr)
        return;

    QueueSong *qSong = new QueueSong;

    qSong->song = song;
    qSong->next = nullptr;

    if (isEmpty())
    {
        frontSong = qSong;
        rearSong = qSong;
    }
    else
    {
        rearSong->next = qSong;
        rearSong = qSong;
    }

    count++;
}

Song *Queue::deQueue()
{
    if (isEmpty())
    {
        return nullptr;
    }

    QueueSong *temp = frontSong;
    Song *song = temp->song;
    frontSong = frontSong->next;

    if (frontSong == nullptr)
    {
        rearSong = nullptr;
    }

    delete temp;
    count--;

    return song;
}

void Queue::clearQueue()
{
    while (frontSong != nullptr)
    {
        QueueSong *temp = frontSong;
        frontSong = frontSong->next;
        delete temp;
    }

    rearSong = nullptr;
    count = 0;
}

void Queue::removeQueueAt(int index)
{
    if (index < 0 || index >= count || frontSong == nullptr)
        return;

    QueueSong *toDelete = nullptr;

    if (index == 0)
    {
        toDelete = frontSong;
        frontSong = frontSong->next;
        if (frontSong == nullptr)
        {
            rearSong = nullptr;
        }
    }
    else
    {
        QueueSong *prev = frontSong;
        for (int i = 0; i < index - 1; ++i)
        {
            prev = prev->next;
        }

        toDelete = prev->next;
        prev->next = toDelete->next;

        if (toDelete == rearSong)
        {
            rearSong = prev;
        }
    }

    delete toDelete;
    count--;
}

std::vector<Song *> Queue::getQueueSongs() const
{
    std::vector<Song *> songs;
    QueueSong *current = frontSong;

    while (current != nullptr)
    {
        songs.push_back(current->song);
        current = current->next;
    }

    return songs;
}
