#pragma once

#include <iostream>
#include <unordered_map>
#include "board/board.hpp"

class BoardReader
{
    public:
    bool ReadBoard(std::istream& stream, BoardState* target, std::unordered_map<int, char>& mapping);
};