#include "solver/cumulative-cost-heuristics/cumulative-zero-cost.hpp"

int CumulativeZeroCost::GetCost(const BoardState& initialBoard, const std::vector<Move>& moves) const
{
    return 0;
}