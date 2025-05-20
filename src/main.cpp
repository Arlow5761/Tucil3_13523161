#include <iostream>
#include <fstream>
#include "board/board.hpp"
#include "solver/solver.hpp"
#include "file-io/board-reader.hpp"
#include "file-io/solution-writer.hpp"
#include "solver/cumulative-cost-heuristics/cumulative-zero-cost.hpp"
#include "solver/cumulative-cost-heuristics/cumulative-move-cost.hpp"
#include "solver/estimated-cost-heuristics/estimated-zero-cost.hpp"
#include "solver/estimated-cost-heuristics/blocking-cars-cost.hpp"

int main()
{
    std::string filename;

    std::cout << "Masukkan nama file konfigurasi:" << std::endl;

    std::cin >> filename;

    std::ifstream fs(filename);

    if (fs.fail())
    {
        std::cout << "Error opening file!" << std::endl;
        return 1;
    }

    BoardState board;

    if (!BoardReader().ReadBoard(fs, &board))
    {
        std::cout << "Error reading file!" << std::endl;
        return 1;
    }

    int method;

    std::cout << "Pilih algoritma yang dipakai (tulis nomornya):\n";
    std::cout << "1. UCS\n";
    std::cout << "2. GBFS\n";
    std::cout << "3. A*\n";

    std::cin >> method;

    Solver solver;
    CumulativeZeroCost cumZero;
    EstimatedZeroCost estZero;
    CumulativeMoveCost cumMove;
    BlockingCarsCost estBlock;

    switch (method)
    {
        case 1:
            solver.SetBacktracking(true);
            solver.SetCumulativeHeuristic(cumMove);
            solver.SetEstimatedHeuristic(estZero);
        break;
        case 2:
            solver.SetBacktracking(false);
            solver.SetCumulativeHeuristic(cumZero);
            solver.SetEstimatedHeuristic(estBlock);
        break;
        case 3:
            solver.SetBacktracking(true);
            solver.SetCumulativeHeuristic(cumMove);
            solver.SetEstimatedHeuristic(estBlock);
        break;
        default:
            std::cout << "Invalid method!";
            return 1;
    }

    solver.SetBoard(board);

    Solution solution = solver.Solve();

    SolutionWriter().WriteSolution(std::cout, solution);

    return 0;
}