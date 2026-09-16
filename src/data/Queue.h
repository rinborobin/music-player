#ifndef QUEUE_H
#define QUEUE_H

#include "Song.h"

class Queue
{
public:
    // void enqueue(Song *song);
    bool isEmpty();
    void addSongToQueue(Song *song);
    Song *deQueue();
    void clearQueue();
    int getQueueSize();
    void removeQueueAt(int index);

private:
    struct QueueSong
    {
        Song *song;
        QueueSong *next = nullptr;
    };

    QueueSong *frontSong = nullptr;
    QueueSong *rearSong = nullptr;
    int count = 0;
};

#endif