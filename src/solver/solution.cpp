#include "solver/solution.hpp"

Solution::Solution(const BoardState& initialBoard, const std::vector<Move> moves) :
initialBoard(initialBoard),
moves(moves)
{}