//
// Created by umbarendill on 10/16/24.
//

#include "AppState.h"

unsigned int AppState::getMenuHeight() const
{
    return this->_menuHeight;
}

unsigned int AppState::getFieldCellSize() const
{
    return this->_fieldCellSize;
}

unsigned int AppState::getRestartButtonSize() const
{
    return this->_restartButtonSize;
}

unsigned int AppState::getRestartButtonX() const
{
    return this->_restartButtonX;
}

unsigned int AppState::getRestartButtonY() const
{
    return this->_restartButtonY;
}

unsigned int AppState::getWindowWidth() const
{
    return this->_windowWidth;
}

unsigned int AppState::getWindowHeight() const
{
    return this->_windowHeight;
}

Game* AppState::getGame()
{
    return &this->_game;
}

void AppState::recalcWindowParams(int minesCount, int fieldWidth, int fieldHeight)
{
    this->_game.resetField(minesCount, fieldWidth, fieldHeight);
    this->_windowWidth = this->_fieldCellSize * this->_game.getFieldWidth();
    this->_windowHeight = this->_fieldCellSize * this->_game.getFieldHeight() + this->_menuHeight;
    this->_restartButtonX = (this->_windowWidth - this->_restartButtonSize) / 2;
    this->_restartButtonY =
        (this->_windowHeight - this->_restartButtonSize - this->_menuHeight) / 2 + this->_menuHeight;
}
