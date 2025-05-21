#include <vector>
#include <unordered_set>
#include "solver/solver.hpp"
#include "board/move.hpp"

Solver::Solver() :
backtracks(true),
inputBoard(nullptr),
cumHeuristic(nullptr),
estHeuristic(nullptr)
{}

Solver::Solver(const Solver& other) :
backtracks(other.backtracks),
inputBoard(nullptr),
cumHeuristic(other.cumHeuristic),
estHeuristic(other.estHeuristic)
{
    if (other.inputBoard != nullptr)
    {
        inputBoard = new BoardState(*other.inputBoard);
    }
}

Solver& Solver::operator=(const Solver& other)
{
    if (&other == this) return *this;

    if (inputBoard != nullptr)
    {
        delete inputBoard;
    }

    backtracks = other.backtracks;
    cumHeuristic = other.cumHeuristic;
    estHeuristic = other.estHeuristic;

    if (other.inputBoard != nullptr)
    {
        inputBoard = new BoardState(*other.inputBoard);
    }

    return *this;
}

Solver::~Solver()
{
    if (inputBoard != nullptr)
    {
        delete inputBoard;
    }
}

void Solver::SetBoard(const BoardState& board)
{
    if (inputBoard != nullptr)
    {
        delete inputBoard;
    }

    inputBoard = new BoardState(board);
}

void Solver::SetBacktracking(const bool boolean)
{
    backtracks = boolean;
}

void Solver::SetCumulativeHeuristic(const CumulativeCostHeuristic& heuristic)
{
    cumHeuristic = &heuristic;
}

void Solver::SetEstimatedHeuristic(const EstimatedCostHeuristic& heuristic)
{
    estHeuristic = &heuristic;
}

struct Node
{
    BoardState finalBoardState;
    std::vector<Move> moves;
    int cumCost;
};

struct NodeHasher
{
    std::size_t operator()(const Node* n) const {
        int sum = 0;
        int width = static_cast<int>(n->finalBoardState.GetWidth());
        int height = static_cast<int>(n->finalBoardState.GetHeight());

        for (int i = 0; i < width * height; i++)
        {
            if (n->finalBoardState.GetPieceIndex(i % width, i / width) == -1)
            {
                sum += i;
            }
        }

        return std::hash<int>()(sum);
    }
};

struct NodeComparer
{
    bool operator()(const Node* a, const Node* b) const {
        return a->finalBoardState == b->finalBoardState;
    }
};

Solution Solver::Solve() const
{
    std::unordered_set<const Node*, NodeHasher, NodeComparer> visitedSet;
    std::unordered_set<const Node*, NodeHasher, NodeComparer> neighbourSet;
    
    const Node* goalNode = new Node{ *inputBoard, {}, 0 };

    neighbourSet.insert(goalNode);

    while (!neighbourSet.empty() && !goalNode->finalBoardState.IsSolved())
    {
        goalNode = nullptr;
        int bestCost = ~(1 << 31);

        for (auto node : neighbourSet)
        {
            int estCost = estHeuristic->GetCost(node->finalBoardState);

            if (node->cumCost + estCost < bestCost)
            {
                bestCost = node->cumCost + estCost;
                goalNode = node;
            }
        }

        neighbourSet.erase(goalNode);
        visitedSet.insert(goalNode);

        if (!backtracks)
        {
            for (auto node : neighbourSet)
            {
                delete node;
            }

            neighbourSet.clear();
        }

        size_t pieceCount = goalNode->finalBoardState.GetPieceCount();
        for (size_t i = 0; i < pieceCount; i++)
        {
            std::pair<int, int> range = goalNode->finalBoardState.GetMoveRange(i);

            for (int j = range.first; j <= range.second; j++)
            {
                if (j == 0) continue;

                Node* newNode = new Node(*goalNode);

                newNode->finalBoardState.MovePiece(i, j);
                newNode->moves.push_back(Move(i, j));
                newNode->cumCost += cumHeuristic->GetCost(*inputBoard, newNode->moves);

                if (visitedSet.find(newNode) == visitedSet.end())
                {
                    neighbourSet.insert(newNode);
                }
                else
                {
                    delete newNode;
                }
            }
        }
    }
    
    return Solution(*inputBoard, goalNode->moves);
}