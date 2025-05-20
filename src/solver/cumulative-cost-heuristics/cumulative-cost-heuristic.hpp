#pragma once

#include <vector>
#include "board/board.hpp"
#include "board/move.hpp"

class CumulativeCostHeuristic
{
    public:
    virtual int GetCost(const BoardState& initialBoard, const std::vector<Move>& moves) const;
};