//
// Created by umbarendill on 10/6/24.
//

#ifndef MINESWEEPER_APPSTATE_H
#define MINESWEEPER_APPSTATE_H

#include "Game.h"

class AppState
{
public:
    AppState(int minesCount, int fieldWidth, int fieldHeight)
    {
        this->_menuHeight = 20;
        this->_fieldCellSize = 48;
        this->_restartButtonSize = this->_fieldCellSize * 2;
        this->_game.resetField(minesCount, fieldWidth, fieldHeight);
        this->_windowWidth = this->_fieldCellSize * this->_game.getFieldWidth();
        this->_windowHeight = this->_fieldCellSize * this->_game.getFieldHeight() + this->_menuHeight;
        this->_restartButtonX = (this->_windowWidth - this->_restartButtonSize) / 2;
        this->_restartButtonY =
            (this->_windowHeight - this->_restartButtonSize - this->_menuHeight) / 2 + this->_menuHeight;
    }

    [[nodiscard]] unsigned int getMenuHeight() const;
    [[nodiscard]] unsigned int getFieldCellSize() const;
    [[nodiscard]] unsigned int getRestartButtonSize() const;
    [[nodiscard]] unsigned int getRestartButtonX() const;
    [[nodiscard]] unsigned int getRestartButtonY() const;
    [[nodiscard]] unsigned int getWindowWidth() const;
    [[nodiscard]] unsigned int getWindowHeight() const;
    Game* getGame();

    void recalcWindowParams(int minesCount, int fieldWidth, int fieldHeight);

private:
    unsigned int _menuHeight;
    unsigned int _fieldCellSize;
    unsigned int _restartButtonSize;
    unsigned int _restartButtonX;
    unsigned int _restartButtonY;
    unsigned int _windowWidth;
    unsigned int _windowHeight;
    Game _game;
};

#endif //MINESWEEPER_APPSTATE_H
