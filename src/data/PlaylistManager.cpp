#include "PlaylistManager.h"
#include "../utils/Logger.h"

void PlaylistManager::createPlaylist(const std::string &name)
{
    Playlist *newPlaylist = new Playlist(name);

    playlists.push_back(newPlaylist);
    LOG_INFO("Created playlist: " + name);

    if (currentPlaylist == nullptr)
    {
        currentPlaylist = newPlaylist;
        LOG_INFO("Set current playlist to: " + name);
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