//
// Created by umbarendill on 10/16/24.
//

#include "Cell.h"

bool Cell::isOpened() const
{
    return this->_isOpened;
}

bool Cell::isFlagged() const
{
    return this->_isFlagged;
}

bool Cell::isQuestioned() const
{
    return this->_isQuestioned;
}

bool Cell::isMine() const
{
    return this->_isMine;
}

unsigned int Cell::getMineNeighboursCount() const
{
    return this->_neighbourMineCount;
}

void Cell::open()
{
    if (this->_isFlagged || this->_isQuestioned) {
        return;
    }

    this->_isOpened = true;
}

void Cell::markFlagged()
{
    this->_isFlagged = true;
    this->_isQuestioned = false;
}

void Cell::markQuestioned()
{
    this->_isQuestioned = true;
    this->_isFlagged = false;
}

void Cell::unmark()
{
    this->_isQuestioned = false;
    this->_isFlagged = false;
}

void Cell::reset(bool isMine)
{
    this->_isOpened = false;
    this->_isFlagged = false;
    this->_isQuestioned = false;
    this->_isMine = isMine;
    this->_neighbourMineCount = 0;
}

void Cell::setMine()
{
    this->_isMine = true;
}

void Cell::addMineNeighbour()
{
    this->_neighbourMineCount++;
}
