#pragma once

#include <iostream>
#include <unordered_map>
#include "solver/solution.hpp"

class SolutionWriter
{
    public:
    void WriteSolution(std::ostream& stream, Solution solution, const std::unordered_map<int, char>& mapping);

    private:
    void WriteBoard(std::ostream& stream, const BoardState& board, const std::unordered_map<int, char>& mapping, int piece);
};