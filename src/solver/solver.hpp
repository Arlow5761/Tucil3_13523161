#pragma once

#include "board/board.hpp"
#include "solver/solution.hpp"
#include "solver/cumulative-cost-heuristics/cumulative-cost-heuristic.hpp"
#include "solver/estimated-cost-heuristics/estimated-cost-heuristic.hpp"

class Solver
{
    public:
    Solver();
    Solver(const Solver& other);
    Solver& operator=(const Solver& other);
    ~Solver();

    void SetBoard(const BoardState& board);
    void SetBacktracking(const bool boolean);
    void SetCumulativeHeuristic(const CumulativeCostHeuristic& heuristic);
    void SetEstimatedHeuristic(const EstimatedCostHeuristic& heuristic);

    Solution Solve() const;
    
    private:
    bool backtracks;
    const BoardState* inputBoard;
    const CumulativeCostHeuristic* cumHeuristic;
    const EstimatedCostHeuristic* estHeuristic;
};