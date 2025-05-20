#pragma once

#include <vector>
#include "board/piece.hpp"

class BoardState
{
    public:
    BoardState();
    BoardState(const size_t width, const size_t height, const int exit);
    BoardState(const BoardState& other);
    BoardState& operator=(const BoardState& other);
    ~BoardState();

    bool operator==(const BoardState& other) const;
    bool operator!=(const BoardState& other) const;

    void AddPiece(const Piece piece, const size_t x, const size_t y);
    void AddPrimaryPiece(const Piece piece, const size_t distance);

    int GetPieceIndex(const size_t x, const size_t y) const;
    int GetPrimaryPieceIndex() const;
    Piece GetPiece(const size_t pieceIndex) const;

    void RemovePiece(const size_t x, const size_t y);
    void RemovePiece(const size_t pieceIndex);

    size_t GetWidth() const;
    size_t GetHeight() const;
    int GetExit() const;

    std::pair<int, int> GetPiecePosition(const size_t pieceIndex) const;
    std::pair<int, int> GetMoveRange(const size_t pieceIndex) const;
    void MovePiece(const size_t index, const int offset);

    size_t GetPieceCount() const;

    bool IsSolved() const;

    private:
    int exit;
    size_t width;
    size_t height;
    int* grid;
    int primaryPiece;
    std::vector<Piece> pieces;
};