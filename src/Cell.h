//
// Created by umbarendill on 10/6/24.
//

#ifndef MINESWEEPER_CELL_H
#define MINESWEEPER_CELL_H

class Cell
{
public:
    [[nodiscard]] bool isOpened() const;
    [[nodiscard]] bool isFlagged() const;
    [[nodiscard]] bool isQuestioned() const;
    [[nodiscard]] bool isMine() const;
    [[nodiscard]] unsigned int getMineNeighboursCount() const;

    void open();
    void markFlagged();
    void markQuestioned();
    void unmark();
    void reset(bool isMine = false);
    void setMine();
    void addMineNeighbour();
private:
    bool _isOpened = false;
    bool _isFlagged = false;
    bool _isQuestioned = false;
    bool _isMine = false;
    unsigned int _neighbourMineCount = 0;
};

#endif //MINESWEEPER_CELL_H
