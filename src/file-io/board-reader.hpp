#pragma once

#include <iostream>
#include "board/board.hpp"

class BoardReader
{
    public:
    bool ReadBoard(std::istream& stream, BoardState* target);
};