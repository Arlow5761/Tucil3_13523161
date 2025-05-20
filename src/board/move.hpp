#pragma once

struct Move
{
    Move(const int piece, const int offset);

    const int piece;
    const int offset;
};