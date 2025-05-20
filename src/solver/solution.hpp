#pragma once

#include <vector>
#include "board/move.hpp"
#include "board/board.hpp"

struct Solution
{
    Solution(const BoardState& initialBoard, const std::vector<Move> moves);

    const BoardState initialBoard;
    const std::vector<Move> moves;
};