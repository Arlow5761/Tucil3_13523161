#pragma once

#include "solver/estimated-cost-heuristics/estimated-cost-heuristic.hpp"

class EstimatedZeroCost : public EstimatedCostHeuristic
{
    public:
    int GetCost(const BoardState& boardState) const;
};