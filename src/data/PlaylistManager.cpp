#include "PlaylistManager.h"
#include "../utils/Logger.h"

PlaylistManager::~PlaylistManager()
{
    for (Playlist *playlist : playlists)
    {
        delete playlist;
    }

    playlists.clear();
    currentPlaylist = nullptr;
}

Playlist *PlaylistManager::createPlaylist(const std::string &name)
{
    Playlist *newPlaylist = new Playlist(name);

    playlists.push_back(newPlaylist);

    if (currentPlaylist == nullptr)
    {
        currentPlaylist = newPlaylist;
    }

    return newPlaylist;
}

std::vector<Playlist *> PlaylistManager::getPlaylists()
{
    return playlists;
}

Playlist *PlaylistManager::getCurrentPlaylist()
{
    return currentPlaylist;
}

void PlaylistManager::selectPlaylist(int index)
{
    if (index < 0 || index >= static_cast<int>(playlists.size()))
        return;

    currentPlaylist = playlists[index];
}
