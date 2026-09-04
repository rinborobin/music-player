cmake_minimum_required(VERSION 3.16)

project(music-player)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG main
)

FetchContent_MakeAvailable(ftxui)

add_executable(music-player
    src/main.cpp
    src/data/Playlist.cpp
    src/audio/MusicEngine.cpp
)

target_link_libraries(music-player
    PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)