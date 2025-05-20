#include <unordered_map>
#include "file-io/solution-writer.hpp"

void SolutionWriter::WriteSolution(std::ostream& stream, Solution solution)
{
    BoardState board(solution.initialBoard);

    std::unordered_map<int, char> mapping;

    mapping.insert(std::make_pair(-1, '.'));
    char curchar = 'A';
    for (int i = 0; i < board.GetPieceCount(); i++)
    {
        if (curchar == 'K' || curchar == 'P')
        {
            curchar++;
        }

        mapping.insert(std::make_pair(i, curchar));
        curchar++;
    }

    stream << "Papan Awal\n";
    WriteBoard(stream, board, mapping, -2);

    for (size_t i = 0; i < solution.moves.size(); i++)
    {
        Move move = solution.moves.at(i);
        board.MovePiece(move.piece, move.offset);

        stream << "Gerakan " << (i + 1) << ":";
        stream << mapping.at(move.piece) << "-";

        if (board.GetPiece(move.piece).GetOrientation() == Piece::Orientation::Horizontal)
        {
            if (move.offset > 0)
            {
                stream << "Kanan";
            }
            else
            {
                stream << "Kiri";
            }
        }
        else
        {
            if (move.offset > 0)
            {
                stream << "Bawah";
            }
            else
            {
                stream << "Atas";
            }
        }

        stream << "\n";

        WriteBoard(stream, board, mapping, move.piece);
    }
}

void SolutionWriter::WriteBoard(std::ostream& stream, const BoardState& board, const std::unordered_map<int, char>& mapping, int piece)
{
    char* grid = new char[(board.GetWidth() + 1) * (board.GetHeight() + 1)];

    for (int i = 0; i < (board.GetWidth() + 1) * (board.GetHeight() + 1); i++)
    {
        grid[i] = ' ';
    }

    int exit = board.GetExit();
    int offsetX = 0;
    int offsetY = 0;

    if (exit < 0)
    {
        return;   
    }
    else if (exit < board.GetWidth())
    {
        offsetY = 1;
        grid[exit] = 'K';
    }
    else if (exit < board.GetWidth() + board.GetHeight())
    {
        grid[(exit - board.GetWidth()) * (board.GetWidth() + 1) + board.GetWidth()] = 'K';
    }
    else if (exit < 2 * board.GetWidth() + board.GetHeight())
    {
        grid[(board.GetHeight() + 1) * (board.GetWidth() + 1) - 1 - exit - board.GetWidth() - board.GetHeight()] = 'K';
    }
    else if (exit < 2 * (board.GetWidth() + board.GetHeight()))
    {
        offsetX = 1;
        grid[(2 * (board.GetWidth() + board.GetHeight()) - 1 - exit) * (board.GetWidth() + 1)] = 'K';
    }
    else
    {
        return;
    }

    for (int j = 0; j < board.GetHeight(); j++)
    {
        for (int i = 0; i < board.GetWidth(); i++)
        {
            int index = board.GetPieceIndex(i, j);

            if (index == -1)
            {
                grid[(offsetY + j) * (board.GetWidth() + 1) + i + offsetX] = '.';
            }
            else if (board.GetPrimaryPieceIndex() == index)
            {
                grid[(offsetY + j) * (board.GetWidth() + 1) + i + offsetX] = 'P';
            }
            else
            {
                grid[(offsetY + j) * (board.GetWidth() + 1) + i + offsetX] = mapping.at(index);
            }
        }
    }

    char coloredChar = -100;
    if (piece >= 0)
    {
        coloredChar = mapping.at(piece);
    }

    for (int j = 0; j < board.GetHeight() + 1; j++)
    {
        for (int i = 0; i < board.GetWidth() + 1; i++)
        {
            char c = grid[j * (board.GetWidth() + 1) + i];

            if (c == coloredChar && &stream == &std::cout)
            {
                stream << "\033[94m" << c << "\033[m";
            }
            else if (c == 'P' && &stream == &std::cout)
            {
                stream << "\033[91m" << c << "\033[m";
            }
            else
            {
                stream << c;
            }
        }

        stream << '\n';
    }
}