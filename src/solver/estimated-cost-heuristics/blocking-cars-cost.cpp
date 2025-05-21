#include "solver/estimated-cost-heuristics/blocking-cars-cost.hpp"

int BlockingCarsCost::GetCost(const BoardState& boardState) const
{
    int primaryPiece = boardState.GetPrimaryPieceIndex();
    
    if (primaryPiece == -1) return 0;
    if (boardState.IsSolved()) return 0;
    
    int x = boardState.GetPiecePosition(primaryPiece).first;
    int y = boardState.GetPiecePosition(primaryPiece).second;
    
    int cost = 0;


    if (boardState.GetExit() < 0)
    {
        return 0;
    }
    else if (boardState.GetExit() < boardState.GetWidth())
    {
        for (int i = y - 1; i >= 0; i--)
        {
            if (boardState.GetPieceIndex(x, i) != -1)
            {
                cost++;
            }
        }
    }
    else if (boardState.GetExit() < boardState.GetWidth() + boardState.GetHeight())
    {
        for (int i = x + boardState.GetPiece(primaryPiece).GetSize(); i < boardState.GetWidth(); i++)
        {
            if (boardState.GetPieceIndex(i, y) != -1)
            {
                cost++;
            }
        }
    }
    else if (boardState.GetExit() < 2 * boardState.GetWidth() + boardState.GetHeight())
    {
        for (int i = y + boardState.GetPiece(primaryPiece).GetSize(); i < boardState.GetHeight(); i++)
        {
            if (boardState.GetPieceIndex(x, i) != -1)
            {
                cost++;
            }
        }
    }
    else if (boardState.GetExit() < 2 * (boardState.GetWidth() + boardState.GetHeight()))
    {
        for (int i = x - 1; i >= 0; i--)
        {
            if (boardState.GetPieceIndex(i, y) != -1)
            {
                cost++;
            }
        }
    }
    
    return cost;
}