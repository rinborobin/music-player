#ifndef DEBUG_CLI_H
#define DEBUG_CLI_H

#include "../audio/MusicEngine.h"
#include "../data/LyricsManager.h"
#include "../data/PlaylistManager.h"
#include "../data/Queue.h"

#include <string>

class DebugCLI
{
public:
    DebugCLI();
    ~DebugCLI();

    void run();

private:
    void printMenu();
    void handleInput(int choice);

    void setupSampleData();
    void testPlay();
    void testPause();
    void testResume();
    void testStop();
    void testNext();
    void testPrevious();
    void testLyrics();
    void testProgress();
    void printCurrentSong();

    void queueAddCurrentSong();
    void queuePlayNext();
    void queuePeek();
    void queueDisplay();
    void queueClear();
    void queueMove();
    void queueRemove();

    void testSortByTitle();
    void testSortByArtist();
    void displayAllSongs();

    void testSearchByTitle();
    void testSearchByArtist();

    bool shouldExit;

    PlaylistManager playlistManager;
    MusicEngine player;
    LyricsManager lyricsManager;
    // Queue playbackQueue;

    std::string sampleSongPath;
    std::string sampleLyricPath;
    std::string sampleTitle;
    std::string sampleArtist;
};

#endif // DEBUG_CLI_H
