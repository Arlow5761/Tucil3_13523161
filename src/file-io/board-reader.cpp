#include <iostream>
#include <string>
#include <unordered_set>
#include "file-io/board-reader.hpp"

bool BoardReader::ReadBoard(std::istream& stream, BoardState* target)
{
    int width, height;

    stream >> width >> height;

    if (width <= 0 || height <= 0)
    {
        return false;
    }

    int pieceCount;

    stream >> pieceCount;

    pieceCount += 1;

    if (pieceCount <= 0)
    {
        return false;
    }

    char* grid = new char[(width + 1) * (height + 1)];

    int j = 0;
    std::string line;
    std::getline(stream, line);
    while (std::getline(stream, line))
    {
        if (j > height)
        {
            delete[] grid;
            return false;
        }

        if (line.size() > width + 1)
        {
            delete[] grid;
            return false;
        }

        for (int i = 0; i < line.size(); i++)
        {
            grid[j * (width + 1) + i] = line.at(i);
        }

        j++;
    }

    int startX = 0;
    int startY = 0;
    int exit = -1;

    if (grid[0] == ' ')
    {
        for (int i = 1; i < width + 1; i++)
        {
            if (grid[i] == 'K')
            {
                startY = 1;
                exit = i;
                break;
            }
        }

        if (exit == -1)
        {
            for (int i = 1; i < height + 1; i++)
            {
                if (grid[i * (width + 1)] == 'K')
                {
                    startX = 1;
                    exit = 2 * (width + height) - 1 - i;
                    break;
                }
            }

            if (exit == -1)
            {
                delete[] grid;
                return false;
            }
        }
    }
    else
    {
        for (int i = 0; i < width; i++)
        {
            if (grid[height * (width + 1) + i] == 'K')
            {
                exit = 2 * (width + height) - height - i - 1;
                break;
            }
        }

        if (exit == -1)
        {
            for (int i = 0; i < height; i++)
            {
                if (grid[i * (width + 1) + width] == 'K')
                {
                    exit = width + i;
                    break;
                }
            }

            if (exit == -1)
            {
                delete[] grid;
                return false;
            }
        }
    }

    *target = BoardState(width, height, exit);

    std::unordered_set<char> seenChars({'.'});

    for (int j = startY; j < startY + height; j++)
    {
        for (int i = startX; i < startX + width; i++)
        {
            char c = grid[j * (width + 1) + i];

            if (c == 'K')
            {
                delete[] grid;
                return false;
            }

            if (seenChars.find(c) != seenChars.end())
            {
                continue;
            }

            seenChars.insert(c);

            Piece::Orientation orientation;
            size_t size = 1;

            if (i == startX + width && j == startY + height)
            {
                delete[] grid;
                return false;
            }

            if (i < startX + width)
            {
                for (int k = i + 1; k < startX + width; k++)
                {
                    char cc = grid[j * (width + 1) + k];

                    if (cc != c)
                    {
                        break;
                    }
                    else
                    {
                        orientation = Piece::Orientation::Horizontal;
                        size += 1;
                    }
                }
            }

            if (size == 1)
            {
                for (int k = j + 1; k < startY + height; k++)
                {
                    char cc = grid[k * (width + 1) + i];

                    if (cc != c)
                    {
                        break;
                    }
                    else
                    {
                        orientation = Piece::Orientation::Vertical;
                        size += 1;
                    }
                }
            }

            if (size == 1)
            {
                delete[] grid;
                return false;
            }

            if (c == 'P')
            {
                int distance;

                if (orientation == Piece::Orientation::Horizontal)
                {
                    if (exit == 2 * (width + height) - (j - startY) - 1)
                    {
                        distance = i - startX;
                    }
                    else if (exit == width + j - startY)
                    {
                        distance = width - (i - startX) - size;
                    }
                    else
                    {
                        delete[] grid;
                        return false;
                    }
                }
                else
                {
                    if (exit == i - startX)
                    {
                        distance = j - startY;
                    }
                    else if (exit == 2 * width + height - (i - startX) - 1)
                    {
                        distance = height - (j - startY) - size;
                    }
                    else
                    {
                        delete[] grid;
                        return false;
                    }
                }

                target->AddPrimaryPiece(Piece(size, orientation), distance);
            }
            else
            {
                target->AddPiece(Piece(size, orientation), i - startX, j - startY);
            }
        }
    }

    return true;
}