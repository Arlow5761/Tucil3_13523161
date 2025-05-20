#pragma once

#include "board/board.hpp"

class EstimatedCostHeuristic
{
    public:
    virtual int GetCost(const BoardState& boardState) const;
};