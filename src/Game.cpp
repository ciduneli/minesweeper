//
// Created by umbarendill on 10/16/24.
//

#include "Game.h"
#include <random>

static std::mt19937 rng(std::random_device{}());

unsigned int Game::getMinesCount() const
{
    return this->_minesCount;
}

unsigned int Game::getFieldWidth() const
{
    return this->_fieldWidth;
}

unsigned int Game::getFieldHeight() const
{
    return this->_fieldHeight;
}

unsigned int Game::getFlagsCount() const
{
    return this->_flagsCount;
}

Uint32 Game::getTime() const
{
    if (this->isFinished()) {
        return (this->_finishTime - this->_startTime) / 1000;
    } else if (this->_isStarted) {
        return (SDL_GetTicks() - this->_startTime) / 1000;
    } else {
        return 0;
    }
}

bool Game::isFinished() const
{
    return this->_isFailed || this->_isSuccess;
}

bool Game::isFailed() const
{
    return this->_isFailed;
}

bool Game::isSuccess() const
{
    return this->_isSuccess;
}

void Game::resetField(int minesCount, int fieldWidth, int fieldHeight)
{
    this->_minesCount = minesCount;
    this->_fieldWidth = fieldWidth;
    this->_fieldHeight = fieldHeight;
}

void Game::startNewGame()
{
    this->_isFailed = false;
    this->_isSuccess = false;
    this->_flagsCount = 0;
    this->_isStarted = false;
    this->_startTime = 0;
    this->_finishTime = 0;

    this->_field.clear();
    this->_fieldBackup.clear();
    this->_field.resize(this->_fieldHeight);
    this->_fieldBackup.resize(this->_fieldHeight);
    for (auto i = 0; i < this->_fieldHeight; i++) {
        this->_field[i].resize(this->_fieldWidth);
        this->_fieldBackup[i].resize(this->_fieldWidth);
        for (auto j = 0; j < this->_fieldWidth; j++) {
            this->_field[i][j].reset();
            this->_fieldBackup[i][j].reset();
        }
    }

    for (auto i = 0; i < this->_minesCount; i++) {
        unsigned int h = std::uniform_int_distribution<unsigned int>(0, this->_fieldHeight - 1)(rng);
        unsigned int w = std::uniform_int_distribution<unsigned int>(0, this->_fieldWidth - 1)(rng);
        if (this->_field[h][w].isMine()) {
            i--;
            continue;
        } else {
            this->_field[h][w].setMine();
            if (h > 0) {
                if (w > 0) { this->_field[h - 1][w - 1].addMineNeighbour(); }
                this->_field[h - 1][w].addMineNeighbour();
                if (w < this->_fieldWidth - 1) { this->_field[h - 1][w + 1].addMineNeighbour(); }
            }
            if (w > 0) { this->_field[h][w - 1].addMineNeighbour(); }
            if (w < this->_fieldWidth - 1) { this->_field[h][w + 1].addMineNeighbour(); }
            if (h < this->_fieldHeight - 1) {
                if (w > 0) { this->_field[h + 1][w - 1].addMineNeighbour(); }
                this->_field[h + 1][w].addMineNeighbour();
                if (w < this->_fieldWidth - 1) { this->_field[h + 1][w + 1].addMineNeighbour(); }
            }
        }
    }
}

void Game::restore()
{
    this->_isStarted = true;
    this->_isFailed = false;
    this->_isSuccess = false;
    this->_field = this->_fieldBackup;
}

void Game::openCell(unsigned int x, unsigned int y)
{
    this->markStarted();
    if (this->_field[x][y].isOpened() || this->_field[x][y].isFlagged() || this->_field[x][y].isQuestioned()) {
        return;
    }

    if (this->_field[x][y].isMine()) {
        this->_fieldBackup = this->_field;
        this->finishWithFail();
    }

    this->_field[x][y].open();
    if (this->isFinished()) {
        return;
    }

    if (this->_field[x][y].getMineNeighboursCount() == 0) {
        if (x > 0) {
            if (y > 0) { this->openCell(x - 1, y - 1); }
            this->openCell(x - 1, y);
            if (y < this->_fieldWidth - 1) { this->openCell(x - 1, y + 1); }
        }
        if (y > 0) { this->openCell(x, y - 1); }
        if (y < this->_fieldWidth - 1) { this->openCell(x, y + 1); }
        if (x < this->_fieldHeight - 1) {
            if (y > 0) { this->openCell(x + 1, y - 1); }
            this->openCell(x + 1, y);
            if (y < this->_fieldWidth - 1) { this->openCell(x + 1, y + 1); }
        }
    }

    bool lastSquare = true;
    for (auto& i : this->_field) {
        if (!lastSquare) {
            break;
        }

        for (auto& j : i) {
            if (!j.isMine() && !j.isOpened()) {
                lastSquare = false;
                break;
            }
        }
    }

    if (lastSquare) {
        this->finishWithSuccess();
    }
}

void Game::markCell(unsigned int x, unsigned int y)
{
    this->markStarted();
    if (this->_field[x][y].isOpened()) {
        return;
    }

    if (!this->_field[x][y].isFlagged() && !this->_field[x][y].isQuestioned()) {
        this->_field[x][y].markFlagged();
        this->_flagsCount++;
    } else if (this->_field[x][y].isFlagged()) {
        this->_field[x][y].markQuestioned();
        this->_flagsCount--;
    } else {
        this->_field[x][y].unmark();
    }
}

Cell* Game::getCell(unsigned int x, unsigned int y)
{
    return &this->_field[x][y];
}
