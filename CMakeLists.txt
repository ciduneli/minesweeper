cmake_minimum_required(VERSION 3.29)
project(minesweeper)

set(CMAKE_CXX_STANDARD 23)

add_executable(minesweeper
        src/Cell.cpp
        src/Game.cpp
        src/AppState.cpp
        main.cpp
        bindings/imgui_impl_sdl2.h
        bindings/imgui_impl_sdl2.cpp
        bindings/imgui_impl_sdlrenderer2.h
        bindings/imgui_impl_sdlrenderer2.cpp
)

find_package(SDL2)
find_package(SDL2_image)
find_package(imgui)
find_package(nlohmann_json)

target_link_libraries(minesweeper SDL2::SDL2 SDL2_image::SDL2_image imgui::imgui nlohmann_json::nlohmann_json)