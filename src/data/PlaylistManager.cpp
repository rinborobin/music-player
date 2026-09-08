#include "PlaylistManager.h"

void PlaylistManager::createPlaylist(const std::string &name)
{
    Playlist *newPlaylist = new Playlist(name);

    playlists.push_back(newPlaylist);

    if (currentPlaylist == nullptr)
    {
        currentPlaylist = newPlaylist;
    }
}

std::vector<Playlist *> PlaylistManager::getPlaylists()
{
    return playlists;
}

Playlist *PlaylistManager::getCurrentPlaylist()
{
    return currentPlaylist;
}