#pragma once

#include "solver/estimated-cost-heuristics/estimated-cost-heuristic.hpp"

class BlockingCarsCost : public EstimatedCostHeuristic
{
    public:
    int GetCost(const BoardState& boardState) const override;
};