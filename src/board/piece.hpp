#pragma once

#include <cstddef>

class Piece
{
    public:
    enum class Orientation
    {
        Horizontal,
        Vertical
    };

    Piece();
    Piece(const size_t size, const Orientation orientation);

    bool operator==(const Piece& other) const;
    bool operator!=(const Piece& other) const;

    size_t GetSize() const;
    Orientation GetOrientation() const;

    private:
    size_t size;
    Orientation orientation;
};