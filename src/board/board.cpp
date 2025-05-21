#include <cassert>
#include "board/board.hpp"

BoardState::BoardState() :
width(0),
height(0),
exit(-1),
primaryPiece(-1),
pieces(0),
grid(nullptr)
{}

BoardState::BoardState(const size_t width, const size_t height, const int exit) :
width(width),
height(height),
exit(exit),
primaryPiece(-1),
pieces()
{
    size_t size = width * height;
    grid = new int[size];
    for (size_t i = 0; i < size; i++) {
        grid[i] = -1;
    }
}

BoardState::BoardState(const BoardState& other) :
width(other.width),
height(other.height),
exit(other.exit),
primaryPiece(other.primaryPiece),
pieces(other.pieces)
{
    size_t size = width * height;
    grid = new int[size];
    for (size_t i = 0; i < size; i++) {
        grid[i] = other.grid[i];
    }
}

BoardState& BoardState::operator=(const BoardState& other)
{
    if (&other == this) return *this;

    if (grid != nullptr) delete[] grid;

    exit = other.exit;
    width = other.width;
    height = other.height;
    primaryPiece = other.primaryPiece;
    pieces = other.pieces;

    size_t size = width * height;
    grid = new int[size];
    for (size_t i = 0; i < size; i++) {
        grid[i] = other.grid[i];
    }

    return *this;
}

BoardState::~BoardState()
{
    if (grid != nullptr) delete[] grid;
}

bool BoardState::operator==(const BoardState& other) const
{
    if (&other == this) return true;

    if (width != other.width || height != other.height) return false;

    for (int i = 0; i < width * height; i++)
    {
        if (grid[i] == primaryPiece && other.grid[i] != primaryPiece) return false;

        if (grid[i] == -1)
        {
            if (other.grid[i] == -1)
            {
                continue;
            }
            else
            {
                return false;
            }
        }

        if (pieces[grid[i]] != other.pieces[other.grid[i]]) return false;
    }

    return true;
}

bool BoardState::operator!=(const BoardState& other) const
{
    return !(*this == other);
}

void BoardState::AddPiece(const Piece piece, const size_t x, const size_t y)
{
    if (x >= width || y >= height)
    {
        return;
    }

    size_t xdiff = 0;
    size_t ydiff = 0;

    switch (piece.GetOrientation())
    {
        case Piece::Orientation::Horizontal:
            xdiff = 1;
        break;
        case Piece::Orientation::Vertical:
            ydiff = 1;
        break;
        default:
            assert(false);
    }

    if (x + xdiff * piece.GetSize() > width || y + ydiff * piece.GetSize() > height)
    {
        return;
    }

    size_t xx = x;
    size_t yy = y;

    for (size_t i = 0; i < piece.GetSize(); i++)
    {
        if (grid[yy * width + xx] != -1)
        {
            for (size_t j = i; j > 0; j--)
            {
                xx -= xdiff;
                yy -= ydiff;

                grid[yy * width + xx] = -1;
            }

            return;
        }

        grid[yy * width + xx] = static_cast<int>(pieces.size());

        xx += xdiff;
        yy += ydiff;
    }

    pieces.push_back(piece);
}

void BoardState::AddPrimaryPiece(const Piece piece, const size_t distance)
{
    size_t x, y = 0;

    if (exit % (width + height) < width)
    {
        if (piece.GetOrientation() != Piece::Orientation::Vertical) return;

        if (exit < width)
        {
            x = exit;
            y = distance;
        }
        else
        {
            x = width - (exit - height - width) - 1;
            y = height - distance - piece.GetSize();
        }
    }
    else
    {
        if (piece.GetOrientation() != Piece::Orientation::Horizontal) return;

        if (exit < width + height)
        {
            x = width - distance - piece.GetSize();
            y = exit - width;
        }
        else
        {
            x = distance;
            y = height - (exit - 2 * width - height) - 1;
        }
    }

    size_t len = pieces.size();

    AddPiece(piece, x, y);

    if (len + 1 == pieces.size())
    {
        primaryPiece = static_cast<int>(len);
    }
}

int BoardState::GetPieceIndex(const size_t x, const size_t y) const
{
    if (x >= width || y >= height) return -1;

    return grid[y * width + x];
}

int BoardState::GetPrimaryPieceIndex() const
{
    return primaryPiece;
}

Piece BoardState::GetPiece(const size_t pieceIndex) const
{
    if (pieceIndex >= pieces.size()) return Piece();

    return pieces[pieceIndex];
}

void BoardState::RemovePiece(const size_t x, const size_t y)
{
    if (x >= width || y >= height) return;

    int idx = grid[y * width + x];

    if (idx == -1) return;

    RemovePiece(idx);
}

void BoardState::RemovePiece(const size_t pieceIndex)
{
    if (pieceIndex >= pieces.size()) return;

    for (int i = 0; i < width * height; i++)
    {
        if (grid[i] > static_cast<int>(pieceIndex))
        {
            grid[i] -= 1;
        }
        else if (grid[i] == static_cast<int>(pieceIndex))
        {
            grid[i] = -1;
        }
    }

    if (primaryPiece > static_cast<int>(pieceIndex))
    {
        primaryPiece -= 1;
    }
    else if (primaryPiece == static_cast<int>(pieceIndex))
    {
        primaryPiece = -1;
    }

    pieces.erase(pieces.begin() + pieceIndex);
}

size_t BoardState::GetWidth() const
{
    return width;
}

size_t BoardState::GetHeight() const
{
    return height;
}

int BoardState::GetExit() const
{
    return exit;
}

std::pair<int, int> BoardState::GetPiecePosition(const size_t pieceIndex) const
{
    for (int i = 0; i < width * height; i++)
    {
        if (grid[i] == pieceIndex)
        {
            return std::make_pair(i % width, i / width);
        }
    }

    return std::make_pair(-1, -1);
}

std::pair<int, int> BoardState::GetMoveRange(const size_t pieceIndex) const
{
    std::pair<int, int> pos = GetPiecePosition(pieceIndex);

    if (pos.first < 0 || pos.second < 0) return std::make_pair(1, -1);

    Piece piece = pieces[pieceIndex];

    int posOffset = 0;
    int negOffset = 0;

    switch (piece.GetOrientation())
    {
        case Piece::Orientation::Horizontal:
            for (int i = pos.first + static_cast<int>(piece.GetSize()); i < static_cast<int>(width); i++)
            {
                if (grid[pos.second * width + i] != -1) break;

                posOffset++;
            }

            for (int i = pos.first - 1; i >= 0; i--)
            {
                if (grid[pos.second * width + i] != -1) break;

                negOffset--;
            }

            return std::make_pair(negOffset, posOffset);
        break;
        case Piece::Orientation::Vertical:
            for (int i = pos.second + static_cast<int>(piece.GetSize()); i < static_cast<int>(height); i++)
            {
                if (grid[i * width + pos.first] != -1) break;

                posOffset++;
            }

            for (int i = pos.second - 1; i >= 0; i--)
            {
                if (grid[i * width + pos.first] != -1) break;

                negOffset--;
            }

            return std::make_pair(negOffset, posOffset);
        break;
        default:
            assert(false);
    }
}

void BoardState::MovePiece(const size_t index, const int offset)
{
    if (index >= pieces.size()) return;

    std::pair<int, int> piecePosition = GetPiecePosition(index);
    std::pair<int, int> moveRange = GetMoveRange(index);

    if (offset < moveRange.first || offset > moveRange.second) return;

    Piece piece = pieces[index];

    switch (piece.GetOrientation())
    {
        case Piece::Orientation::Horizontal:
            if (offset > 0)
            {
                int start = piecePosition.first + piece.GetSize() - 1;
                for (int i = start; i >= piecePosition.first; i--)
                {
                    grid[piecePosition.second * width + i] = -1;
                    grid[piecePosition.second * width + i + offset] = index;
                }
            }
            else
            {
                int end = piecePosition.first + piece.GetSize();
                for (int i = piecePosition.first; i < end; i++)
                {
                    grid[piecePosition.second * width + i] = -1;
                    grid[piecePosition.second * width + i + offset] = index;
                }
            }
        break;
        case Piece::Orientation::Vertical:
            if (offset > 0)
            {
                int start = piecePosition.second + piece.GetSize() - 1;
                for (int i = start; i >= piecePosition.second; i--)
                {
                    grid[i * width + piecePosition.first] = -1;
                    grid[(i + offset) * width + piecePosition.first] = index;
                }
            }
            else
            {
                int end = piecePosition.second + piece.GetSize();
                for (int i = piecePosition.second; i < end; i++)
                {
                    grid[i * width + piecePosition.first] = -1;
                    grid[(i + offset) * width + piecePosition.first] = index;
                }
            }
        break;
        default:
            assert(false);
    }
}

size_t BoardState::GetPieceCount() const
{
    return pieces.size();
}

bool BoardState::IsSolved() const
{
    size_t x, y = 0;

    if (exit % (width + height) < width)
    {
        if (exit < width)
        {
            x = exit;
        }
        else
        {
            x = width - (exit - height - width) - 1;
            y = height - 1;
        }
    }
    else
    {
        if (exit < width + height)
        {
            x = width - 1;
            y = exit - width;
        }
        else
        {
            y = height - (exit - 2 * width - height) - 1;
        }
    }

    return grid[y * width + x] == primaryPiece && primaryPiece != -1;
}