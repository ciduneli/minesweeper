#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>

#include "imgui.h"
#include "bindings/imgui_impl_sdl2.h"
#include "bindings/imgui_impl_sdlrenderer2.h"

#include "src/AppState.h"

struct LeaderboardItem
{
    int time = 0;
    std::string name = "none";

    bool operator<(const LeaderboardItem& other) const
    {
        return (time < other.time);
    }
};
typedef std::set<LeaderboardItem> Leaderboard;
typedef std::map<std::string, Leaderboard> Leaderboards;
std::string leaderboardFilename = "../leaderboard";
std::string currentSize = "Small";

Leaderboards generateDefaultLeaderboards()
{
    Leaderboards leaderboards;
    leaderboards.insert(std::pair(std::string("Small"), Leaderboard()));
    leaderboards.insert(std::pair(std::string("Medium"), Leaderboard()));
    leaderboards.insert(std::pair(std::string("Big"), Leaderboard()));

    return leaderboards;
}

Leaderboards readLeaderboards()
{
    std::ifstream leaderboardFile(leaderboardFilename);
    if (!leaderboardFile.is_open()) {
        throw std::runtime_error("Could not open the leaderboard file!");
    }

    if (leaderboardFile.peek() == std::ifstream::traits_type::eof()) {
        throw std::runtime_error("Leaderboard file is empty!");
    }

    nlohmann::json jsonData;
    leaderboardFile >> jsonData;

    // Проверяем, что данные не пусты
    if (jsonData.is_null() || jsonData.empty()) {
        throw std::runtime_error("Leaderboard JSON data is empty!");
    }

    Leaderboards leaderboards;
    leaderboards.insert(std::pair(std::string("Small"), Leaderboard()));
    leaderboards.insert(std::pair(std::string("Medium"), Leaderboard()));
    leaderboards.insert(std::pair(std::string("Big"), Leaderboard()));

    auto small = jsonData.at("Small").get<std::vector<nlohmann::json> >();
    for (const auto& it : small) {
        leaderboards["Small"].insert(LeaderboardItem(
            it.at("time").get<int>(),
            it.at("name").get<std::string>()
        ));
    }
    auto medium = jsonData.at("Medium").get<std::vector<nlohmann::json> >();
    for (const auto& it : medium) {
        leaderboards["Medium"].insert(LeaderboardItem(
            it.at("time").get<int>(),
            it.at("name").get<std::string>()
        ));
    }
    auto big = jsonData.at("Big").get<std::vector<nlohmann::json> >();
    for (const auto& it : big) {
        leaderboards["Big"].insert(LeaderboardItem(
            it.at("time").get<int>(),
            it.at("name").get<std::string>()
        ));
    }

    return leaderboards;
}

void saveLeaderboards(Leaderboards leaderboards)
{
    std::ofstream outputFile(leaderboardFilename);
    if (!outputFile.is_open()) {
        throw std::runtime_error("Could not open the leaderboard file!");
    }

    nlohmann::json jsonData;
    jsonData["Small"] = std::vector<nlohmann::json>();
    for (const auto& it : leaderboards["Small"]) {
        nlohmann::json item = {
            { "name", it.name },
            { "time", it.time }
        };
        jsonData["Small"].push_back(item);
    }
    jsonData["Medium"] = std::vector<nlohmann::json>();
    for (const auto& it : leaderboards["Medium"]) {
        nlohmann::json item = {
            { "name", it.name },
            { "time", it.time }
        };
        jsonData["Medium"].push_back(item);
    }
    jsonData["Big"] = std::vector<nlohmann::json>();
    for (const auto& it : leaderboards["Big"]) {
        nlohmann::json item = {
            { "name", it.name },
            { "time", it.time }
        };
        jsonData["Big"].push_back(item);
    }

    outputFile << jsonData.dump(4);
}

// Main code
int main(int, char**)
{
    AppState appState(10, 8, 8);

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    auto window_flags = SDL_WINDOW_ALLOW_HIGHDPI;

    SDL_Window* window = SDL_CreateWindow(
        "Minesweeper",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        static_cast<int>(appState.getWindowWidth()),
        static_cast<int>(appState.getWindowHeight()),
        window_flags
    );
    if (window == nullptr) {
        std::cerr << "Error: SDL_CreateWindow(): " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Error: Error creating SDL_Renderer!: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    auto texture = IMG_LoadTexture(renderer, "../tiles.png");
    auto src_closed = SDL_Rect(16 * 0, 16 * 0, 16, 16);
    auto src_flagged = SDL_Rect(16 * 1, 16 * 0, 16, 16);
    auto src_questioned = SDL_Rect(16 * 2, 16 * 0, 16, 16);
    auto src_smile = SDL_Rect(16 * 3, 16 * 0, 16, 16);
    auto src_dead = SDL_Rect(16 * 4, 16 * 0, 16, 16);
    auto src_one = SDL_Rect(16 * 0, 16 * 1, 16, 16);
    auto src_two = SDL_Rect(16 * 1, 16 * 1, 16, 16);
    auto src_three = SDL_Rect(16 * 2, 16 * 1, 16, 16);
    auto src_four = SDL_Rect(16 * 3, 16 * 1, 16, 16);
    auto src_empty = SDL_Rect(16 * 4, 16 * 1, 16, 16);
    auto src_five = SDL_Rect(16 * 0, 16 * 2, 16, 16);
    auto src_six = SDL_Rect(16 * 1, 16 * 2, 16, 16);
    auto src_seven = SDL_Rect(16 * 2, 16 * 2, 16, 16);
    auto src_eight = SDL_Rect(16 * 3, 16 * 2, 16, 16);
    auto src_mine = SDL_Rect(16 * 4, 16 * 2, 16, 16);
    auto dst = SDL_Rect(0, 0, static_cast<int>(appState.getFieldCellSize()), static_cast<int>(appState.getFieldCellSize()));

    Leaderboards leaderboards;
    try {
        leaderboards = readLeaderboards();
    }
    catch ([[maybe_unused]] std::runtime_error& e) {
        leaderboards = generateDefaultLeaderboards();
    }
    catch (nlohmann::json::parse_error& e) {
        std::string error = "Leaderboard file parse error: ";
        error.append(e.what());
        std::cerr << error << std::endl;
        return -1;
    }
    catch (nlohmann::json::out_of_range& e) {
        std::string error = "Leaderboard JSON key error: ";
        error.append(e.what());
        std::cerr << error << std::endl;
        return -1;
    }
    catch (nlohmann::json::type_error& e) {
        std::string error = "Leaderboard JSON type error: ";
        error.append(e.what());
        std::cerr << error << std::endl;
        return -1;
    }
    bool showAddToLeaderboard = false;
    bool showLeaderboard = false;

    auto game = appState.getGame();
    game->startNewGame();

    // Main loop
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
                continue;
            }
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (
                event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
                && event.window.windowID == SDL_GetWindowID(window)
            ) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_r) {
                    game->startNewGame();
                }
                if (event.key.keysym.sym == SDLK_q) {
                    quit = true;
                }
                if (event.key.keysym.sym == SDLK_l) {
                    showLeaderboard = true;
                }
                if (game->isFailed() && event.key.keysym.sym == SDLK_z) {
                    game->restore();
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.y <= appState.getMenuHeight()) {
                    continue;
                }
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (
                        game->isFinished()
                        && event.button.x >= appState.getRestartButtonX()
                        && event.button.x <= appState.getRestartButtonX() + appState.getRestartButtonSize()
                        && event.button.y >= appState.getRestartButtonY()
                        && event.button.y <= appState.getRestartButtonY() + appState.getRestartButtonSize()
                    ) {
                        game->startNewGame();
                        continue;
                    }
                    if (game->isFinished()) {
                        continue;
                    }
                    game->openCell(
                        (event.button.y - appState.getMenuHeight()) / appState.getFieldCellSize(),
                        event.button.x / appState.getFieldCellSize()
                    );
                    if (game->isSuccess()) {
                        if (leaderboards[currentSize].empty() || leaderboards[currentSize].size() < 10) {
                            showAddToLeaderboard = true;
                            continue;
                        }

                        auto lastLeader = --leaderboards[currentSize].end();
                        if (lastLeader->time > static_cast<int>(game->getTime())) {
                            showAddToLeaderboard = true;
                        }
                    }
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    if (game->isFinished()) {
                        continue;
                    }
                    game->markCell(
                        (event.button.y - appState.getMenuHeight()) / appState.getFieldCellSize(),
                        event.button.x / appState.getFieldCellSize()
                    );
                }
            }
        }

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Restart", "R")) {
                game->startNewGame();
            }
            if (game->isFailed()) {
                if (ImGui::MenuItem("Undo", "Z")) {
                    game->restore();
                }
            }
            if (ImGui::MenuItem("Leaderboard", "L")) {
                showLeaderboard = true;
            }
            if (ImGui::BeginMenu("Field size")) {
                if (ImGui::MenuItem("Small")) {
                    currentSize = "Small";
                    appState.recalcWindowParams(10, 8, 8);
                    SDL_SetWindowSize(window, static_cast<int>(appState.getWindowWidth()), static_cast<int>(appState.getWindowHeight()));
                    game->startNewGame();
                }
                if (ImGui::MenuItem("Medium")) {
                    currentSize = "Medium";
                    appState.recalcWindowParams(40, 16, 16);
                    SDL_SetWindowSize(window, static_cast<int>(appState.getWindowWidth()), static_cast<int>(appState.getWindowHeight()));
                    game->startNewGame();
                }
                if (ImGui::MenuItem("Big")) {
                    currentSize = "Big";
                    appState.recalcWindowParams(99, 30, 16);
                    SDL_SetWindowSize(window, static_cast<int>(appState.getWindowWidth()), static_cast<int>(appState.getWindowHeight()));
                    game->startNewGame();
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Exit", "Q")) {
                quit = true;
            }
            ImGui::EndMenu();
        }

        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

        if (showAddToLeaderboard) {
            ImGui::OpenPopup("Add To Leaderboard");
        }

        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Add To Leaderboard", nullptr, ImGuiChildFlags_AlwaysAutoResize)) {
            static char buf[32] = "";
            ImGui::InputText("Nickname", buf, 32);
            ImGui::Separator();
            if (ImGui::Button("Cancel", ImVec2(60.0f, 0))) {
                showAddToLeaderboard = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("OK", ImVec2(60.0f, 0))) {
                std::string nickname(buf);
                if (nickname.empty()) {
                    nickname = "Anonym";
                }
                leaderboards[currentSize].insert(LeaderboardItem(static_cast<int>(game->getTime()), nickname));
                if (leaderboards[currentSize].size() > 10) {
                    auto lastLeader = --leaderboards[currentSize].end();
                    leaderboards[currentSize].erase(lastLeader);
                }
                showAddToLeaderboard = false;
                showLeaderboard = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (showLeaderboard) {
            ImGui::OpenPopup("Leaderboard");
        }

        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Leaderboard", nullptr, ImGuiChildFlags_AlwaysAutoResize)) {
            if (ImGui::BeginTabBar("LeaderboardTabBar")) {
                if (ImGui::BeginTabItem("Small")) {
                    int i = 1;
                    for (const auto& it : leaderboards["Small"]) {
                        ImGui::Text("%d. %s - %ds", i, it.name.c_str(), it.time);
                        i++;
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Medium")) {
                    int i = 1;
                    for (const auto& it : leaderboards["Medium"]) {
                        ImGui::Text("%d. %s - %ds", i, it.name.c_str(), it.time);
                        i++;
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Big")) {
                    int i = 1;
                    for (const auto& it : leaderboards["Big"]) {
                        ImGui::Text("%d. %s - %ds", i, it.name.c_str(), it.time);
                        i++;
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::Separator();

            float centerX = (ImGui::GetContentRegionAvail().x - 120) / 2 + ImGui::GetStyle().WindowPadding.x;
            ImGui::SetCursorPosX(centerX);
            // Кнопка закрытия окна
            if (ImGui::Button("OK", ImVec2(120.0f, 0))) {
                showLeaderboard = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        std::string flagCountText = std::to_string(game->getFlagsCount());
        std::string minesCountText = std::to_string(game->getMinesCount());
        std::string time = std::to_string(game->getTime());
        std::string text = flagCountText.append("/").append(minesCountText).append(" | ").append(time).append("s");
        ImGui::SameLine(
            ImGui::GetWindowWidth()
            - ImGui::CalcTextSize(text.c_str()).x
            - ImGui::GetStyle().ItemSpacing.x
        );
        ImGui::Text("%s", text.c_str());
        ImGui::EndMainMenuBar();

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

        SDL_RenderClear(renderer);

        dst.w = static_cast<int>(appState.getFieldCellSize());
        dst.h = static_cast<int>(appState.getFieldCellSize());
        for (int i = 0; i < game->getFieldHeight(); i++) {
            for (int j = 0; j < game->getFieldWidth(); j++) {
                dst.x = j * static_cast<int>(appState.getFieldCellSize());
                dst.y = i * static_cast<int>(appState.getFieldCellSize()) + static_cast<int>(appState.getMenuHeight());
                auto cell = game->getCell(i, j);
                if (cell->isFlagged()) {
                    SDL_RenderCopy(renderer, texture, &src_flagged, &dst);
                } else if (cell->isQuestioned()) {
                    SDL_RenderCopy(renderer, texture, &src_questioned, &dst);
                } else if (!cell->isOpened()) {
                    SDL_RenderCopy(renderer, texture, &src_closed, &dst);
                } else if (cell->isMine()) {
                    SDL_RenderCopy(renderer, texture, &src_mine, &dst);
                } else if (cell->getMineNeighboursCount() == 0) {
                    SDL_RenderCopy(renderer, texture, &src_empty, &dst);
                } else if (cell->getMineNeighboursCount() == 1) {
                    SDL_RenderCopy(renderer, texture, &src_one, &dst);
                } else if (cell->getMineNeighboursCount() == 2) {
                    SDL_RenderCopy(renderer, texture, &src_two, &dst);
                } else if (cell->getMineNeighboursCount() == 3) {
                    SDL_RenderCopy(renderer, texture, &src_three, &dst);
                } else if (cell->getMineNeighboursCount() == 4) {
                    SDL_RenderCopy(renderer, texture, &src_four, &dst);
                } else if (cell->getMineNeighboursCount() == 5) {
                    SDL_RenderCopy(renderer, texture, &src_five, &dst);
                } else if (cell->getMineNeighboursCount() == 6) {
                    SDL_RenderCopy(renderer, texture, &src_six, &dst);
                } else if (cell->getMineNeighboursCount() == 7) {
                    SDL_RenderCopy(renderer, texture, &src_seven, &dst);
                } else if (cell->getMineNeighboursCount() == 8) {
                    SDL_RenderCopy(renderer, texture, &src_eight, &dst);
                }
            }
        }

        dst.x = static_cast<int>(appState.getRestartButtonX());
        dst.y = static_cast<int>(appState.getRestartButtonY());
        dst.w = static_cast<int>(appState.getRestartButtonSize());
        dst.h = static_cast<int>(appState.getRestartButtonSize());
        if (game->isFailed()) {
            SDL_RenderCopy(renderer, texture, &src_dead, &dst);
        } else if (game->isSuccess()) {
            SDL_RenderCopy(renderer, texture, &src_smile, &dst);
        }

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    saveLeaderboards(leaderboards);

    SDL_DestroyTexture(texture);

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
