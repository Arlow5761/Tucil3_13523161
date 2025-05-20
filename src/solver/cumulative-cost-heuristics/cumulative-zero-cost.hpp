#pragma once

#include "solver/cumulative-cost-heuristics/cumulative-cost-heuristic.hpp"

class CumulativeZeroCost : public CumulativeCostHeuristic
{
    public:
    int GetCost(const BoardState& initialBoard, const std::vector<Move>& moves) const override;
};