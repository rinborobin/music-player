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

int Queue::getQueueSize() { return count; }

void Queue::addSongToQueue(Song *song)
{
    // Song *newSong = new Song;

    QueueSong *qSong = new QueueSong;

    qSong->song = song;
    qSong->next = nullptr;

    // std::cout << isEmpty();

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
};

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
        rearSong == nullptr;
    delete temp;
    count--;

    return song;
}
void Queue::clearQueue()
{
    if (isEmpty())
    {
        return;
    }
    while (frontSong != nullptr)
    {
        QueueSong *temp = frontSong;
        frontSong = frontSong->next;
        if (frontSong == nullptr)
            rearSong = nullptr;
        delete temp;
        count--;
    }
    // std::cout << "H";
};

void Queue::removeQueueAt(int index)
{
    int t_index = 0;

    QueueSong *cur = frontSong;
    QueueSong *prev = frontSong;
    while (t_index != index - 1)
    {
        prev = prev->next;
        cur = prev;
    }
    cur = cur->next;
    prev->next = cur->next;
    delete cur;
    count--;
}
