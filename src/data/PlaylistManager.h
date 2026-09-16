#ifndef PLAYLIST_MANAGER_H
#define PLAYLIST_MANAGER_H

#include <string>
#include <vector>
#include "Playlist.h"

class PlaylistManager
{
public:
    Playlist *createPlaylist(const std::string &name);

    std::vector<Playlist *> getPlaylists();

    Playlist *getCurrentPlaylist();

    void selectPlaylist(int index);

private:
    std::vector<Playlist *> playlists;
    Playlist *currentPlaylist = nullptr;
};

#endif