//
// Created by umbarendill on 10/6/24.
//

#ifndef MINESWEEPER_GAME_H
#define MINESWEEPER_GAME_H

#include <SDL.h>
#include <vector>

#include "Cell.h"

class Game
{
public:
    [[nodiscard]] unsigned int getMinesCount() const;
    [[nodiscard]] unsigned int getFieldWidth() const;
    [[nodiscard]] unsigned int getFieldHeight() const;
    [[nodiscard]] unsigned int getFlagsCount() const;
    [[nodiscard]] Uint32 getTime() const;
    [[nodiscard]] bool isFinished() const;
    [[nodiscard]] bool isFailed() const;
    [[nodiscard]] bool isSuccess() const;

    void resetField(int minesCount, int fieldWidth, int fieldHeight);
    void startNewGame();
    void restore();
    void openCell(unsigned int x, unsigned int y);
    void markCell(unsigned int x, unsigned int y);
    Cell* getCell(unsigned int x, unsigned int y);
private:
    void markStarted()
    {
        if (!this->_isStarted) {
            this->_isStarted = true;
            this->_startTime = SDL_GetTicks();
            this->_finishTime = SDL_GetTicks();
        }
    }

    void finishWithSuccess()
    {
        this->_finishTime = SDL_GetTicks();
        this->_isStarted = false;
        this->_isSuccess = true;
        this->_isFailed = false;
    }

    void finishWithFail()
    {
        this->_finishTime = SDL_GetTicks();
        this->_isStarted = false;
        this->_isSuccess = false;
        this->_isFailed = true;

        for (auto& i : this->_field) {
            for (auto& j : i) {
                if (j.isMine()) {
                    j.open();
                }
            }
        }
    }

    unsigned int _minesCount = 10;
    unsigned int _fieldWidth = 8;
    unsigned int _fieldHeight = 8;
    bool _isStarted = false;
    bool _isFailed = false;
    bool _isSuccess = false;
    unsigned int _flagsCount = 0;
    Uint32 _startTime = 0;
    Uint32 _finishTime = 0;

    std::vector<std::vector<Cell>> _field;
    std::vector<std::vector<Cell>> _fieldBackup;
};

#endif //MINESWEEPER_GAME_H
