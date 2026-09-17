#ifndef QUEUE_H
#define QUEUE_H

#include "Song.h"
#include <vector>

class Queue
{
public:
    bool isEmpty();
    int getQueueSize();
    void addSongToQueue(Song *song);
    Song *deQueue();
    void clearQueue();
    void removeQueueAt(int index);
    std::vector<Song *> getQueueSongs() const;

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
