#include "board/piece.hpp"

Piece::Piece() :
size(0),
orientation(Orientation::Horizontal)
{}

Piece::Piece(const size_t size, const Orientation orientation) :
size(size),
orientation(orientation)
{}

bool Piece::operator==(const Piece& other) const
{
    return size == other.size && orientation == other.orientation;
}

bool Piece::operator!=(const Piece& other) const
{
    return size != other.size || orientation != other.orientation;
}

size_t Piece::GetSize() const
{
    return size;
}

Piece::Orientation Piece::GetOrientation() const
{
    return orientation;
}