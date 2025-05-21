#include <cassert>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

#include "reasings.h"

#include "board/board.hpp"
#include "solver/solver.hpp"
#include "solver/solution.hpp"
#include "file-io/board-reader.hpp"
#include "file-io/solution-writer.hpp"
#include "solver/cumulative-cost-heuristics/cumulative-move-cost.hpp"
#include "solver/cumulative-cost-heuristics/cumulative-zero-cost.hpp"
#include "solver/estimated-cost-heuristics/blocking-cars-cost.hpp"
#include "solver/estimated-cost-heuristics/estimated-zero-cost.hpp"

struct Car
{
    Piece piece;
    float xCurrent;
    float yCurrent;
    float lerp;
    float xPast;
    float yPast;
};

enum class State
{
    Configuring,
    ShowingResult
};

int main()
{
    const int screenWidth = 1000;
    const int screenHeight = 600;
    const int sidebarWidth = 250;

    State state = State::Configuring;

    std::string filename = "No File Selected";
    BoardState* board = nullptr;
    std::unordered_map<int, Car> cars;
    std::unordered_map<int, char> carLabels;
    int selectedAlgorithm = 0;
    bool dropdownActive = false;
    
    Solution* solution = nullptr;
    std::vector<const char*> moveLabels;
    int moveScrollIndex = 0;
    int moveListActive = -1;
    int moveListFocus = -1;
    int moveIndex = 0;
    int highlightedCar = -1;
    bool autoEnabled = false;
    float timer = 0;

    InitWindow(screenWidth, screenHeight, "Rush Hour Solver");

    GuiWindowFileDialogState fileDialog = InitGuiWindowFileDialog(GetWorkingDirectory());

    // Camera settings
    float radius = 10.0f;               // Distance from the origin
    float theta = 135.0f * DEG2RAD;     // Horizontal angle
    float phi = 45.0f * DEG2RAD;        // Vertical angle

    Camera3D camera = { 0 };
    camera.target = { 0.0f, 0.0f, 0.0f };     // Always look at origin
    camera.up = { 0.0f, 1.0f, 0.0f };         // Up is always Y+
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    RenderTexture screenCamera = LoadRenderTexture(screenWidth - sidebarWidth, screenHeight);
    Rectangle cameraRect = { 0.0f, 0.0f, (float)screenCamera.texture.width, (float)-screenCamera.texture.height };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Capture mouse only when inside the 3D viewport and outside file dialog
        Vector2 mouse = GetMousePosition();
        bool in3DViewport = (mouse.x > sidebarWidth);
        bool outsideFileDialog = !fileDialog.windowActive || (
        (mouse.x < fileDialog.windowBounds.x || mouse.x > fileDialog.windowBounds.x + fileDialog.windowBounds.width) &&
        (mouse.y < fileDialog.windowBounds.y || mouse.y > fileDialog.windowBounds.y + fileDialog.windowBounds.height));

        if (in3DViewport && outsideFileDialog)
        {
            // Mouse drag to rotate
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                Vector2 delta = GetMouseDelta();
                theta += delta.x * 0.01f;
                phi -= delta.y * 0.01f;

                // Clamp vertical angle to avoid flipping
                const float epsilon = 0.01f;
                phi = std::clamp(phi, epsilon, PI - epsilon);
            }

            // Mouse wheel to zoom
            float scroll = GetMouseWheelMove();
            radius -= scroll;
            radius = std::clamp(radius, 2.0f, 50.0f);
        }

        // Spherical to Cartesian conversion
        float x = radius * sinf(phi) * cosf(theta);
        float y = radius * cosf(phi);
        float z = radius * sinf(phi) * sinf(theta);
        camera.position = { x, y, z };

        // Render camera
        BeginTextureMode(screenCamera);
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                if (board != nullptr)
                {
                    DrawCube({ 0.0f, 0.1f, 0.0f }, 1.9f + board->GetWidth(), 0.2f, 1.9f + board->GetHeight(), DARKGRAY);

                    int width = static_cast<int>(board->GetWidth());
                    int height = static_cast<int>(board->GetHeight());
                    
                    for (int i = 0; i < 2 * (width + height); i++)
                    {
                        if (i == board->GetExit()) continue;

                        float x, y;

                        if (i < width)
                        {
                            x = static_cast<float>(-width) / 2 + 0.5f + i;
                            y = static_cast<float>(-height) / 2 + 0.5f - 1;
                        }
                        else if (i < width + height)
                        {
                            x = static_cast<float>(width) / 2 - 0.5f + 1;
                            y = static_cast<float>(-height) / 2 + 0.5f + (i - width);
                        }
                        else if (i < 2 * width + height)
                        {
                            x = static_cast<float>(width) / 2 - 0.5f - (i - height - width);
                            y = static_cast<float>(height) / 2 - 0.5f + 1;
                        }
                        else
                        {
                            x = static_cast<float>(-width) / 2 + 0.5f - 1;
                            y = static_cast<float>(height) / 2 - 0.5f - (i - 2 * width - height);
                        }

                        DrawCube({ x, 0.1f, y }, 0.95f, 0.4f, 0.95f, GRAY);
                    }

                    DrawCube({ (float)-width / 2 - 0.5f, 0.1f, (float)-height / 2 - 0.5f }, 0.95f, 0.4f, 0.95f, GRAY);
                    DrawCube({ (float)width / 2 + 0.5f, 0.1f, (float)-height / 2 - 0.5f }, 0.95f, 0.4f, 0.95f, GRAY);
                    DrawCube({ (float)width / 2 + 0.5f, 0.1f, (float)height / 2 + 0.5f }, 0.95f, 0.4f, 0.95f, GRAY);
                    DrawCube({ (float)-width / 2 - 0.5f, 0.1f, (float)height / 2 + 0.5f }, 0.95f, 0.4f, 0.95f, GRAY);

                    for (auto& pair : cars)
                    {
                        if (pair.second.lerp < 1.0f)
                        {
                            pair.second.lerp += GetFrameTime();
                        }

                        float x = EaseExpoInOut(pair.second.lerp, pair.second.xPast, pair.second.xCurrent - pair.second.xPast, 1.0f);
                        float y = EaseExpoInOut(pair.second.lerp, pair.second.yPast, pair.second.yCurrent - pair.second.yPast, 1.0f);

                        float width, length;
                        float headWidth, headLength;
                        switch (pair.second.piece.GetOrientation())
                        {
                            case Piece::Orientation::Horizontal:
                                headLength = length = 0.8f;
                                width = static_cast<float>(pair.second.piece.GetSize()) - 0.4f;
                                headWidth = width - 1.0f;
                            break;
                            case Piece::Orientation::Vertical:
                                length = static_cast<float>(pair.second.piece.GetSize()) - 0.4f;
                                headLength = length - 1.0f;
                                headWidth = width = 0.8f;
                            break;
                        }

                        Color color = BLUE;
                        if (pair.first == board->GetPrimaryPieceIndex())
                        {
                            color = YELLOW;
                        }
                        else if (pair.first == highlightedCar || pair.second.lerp < 1.0f)
                        {
                            color = RED;
                        }

                        DrawCube({ x, 0.3f, y }, width, 0.4f, length, color);
                        DrawCube({ x, 0.5f, y }, headWidth, 0.4f, headLength, color);
                    }
                }
                else
                {
                    DrawGrid(100, 1.0f);
                }
            EndMode3D();

        EndTextureMode();

        if (fileDialog.SelectFilePressed)
        {
            filename = fileDialog.fileNameText;

            std::string dirpath(fileDialog.dirPathText);
            std::string filesep("/");
            std::string filepath = dirpath + filesep + filename;
            
            switch (state)
            {
                case State::Configuring:
                {
                    std::ifstream filestream(filepath);

                    cars.clear();
                    carLabels.clear();
                    if (!filestream.fail())
                    {
                        BoardState* newBoard = new BoardState();
                        if (BoardReader().ReadBoard(filestream, newBoard, carLabels))
                        {
                            if (board != nullptr) delete board;
                            board = newBoard;

                            for (int i = 0; i < board->GetPieceCount(); i++)
                            {
                                Piece piece = board->GetPiece(i);
                                std::pair<int, int> pos = board->GetPiecePosition(i);

                                float x = -static_cast<float>(board->GetWidth()) / 2 + 0.5f + pos.first;
                                float y = -static_cast<float>(board->GetHeight()) / 2 + 0.5f + pos.second;

                                switch (piece.GetOrientation())
                                {
                                    case Piece::Orientation::Horizontal:
                                        x += static_cast<float>(piece.GetSize()) / 2 - 0.5f;
                                    break;
                                    case Piece::Orientation::Vertical:
                                        y += static_cast<float>(piece.GetSize()) / 2 - 0.5f;
                                    break;
                                }

                                cars.insert(std::make_pair(i, Car{
                                    .piece = piece,
                                    .xCurrent = x,
                                    .yCurrent = y,
                                    .lerp = 1.0f,
                                    .xPast = x,
                                    .yPast = y,
                                }));
                            }
                        }
                        else
                        {
                            delete newBoard;
                            if (board != nullptr)
                            {
                                delete board;
                                board = nullptr;
                            }

                            filename = "Error reading file!";
                        }
                    }
                    else
                    {
                        if (board != nullptr)
                        {
                            delete board;
                            board = nullptr;
                        }

                        filename = "Error opening file!";
                    }
                }
                break;
                case State::ShowingResult:
                {
                    std::ofstream filestream(filepath);

                    if (!filestream.fail())
                    {
                        SolutionWriter().WriteSolution(filestream, *solution, carLabels);
                    }
                }
                break;
            }

            fileDialog.windowActive = false;
            fileDialog.SelectFilePressed = false;
        }

        // Drawing
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // Draw camera
            DrawTextureRec(screenCamera.texture, cameraRect, (Vector2){ sidebarWidth, 0 }, WHITE);
            DrawText("Right-click and drag to rotate, scroll to zoom", sidebarWidth + 10, screenHeight - 30, 20, DARKGRAY);

            // Compass settings
            const int compassRadius = 40;
            const Vector2 compassCenter = { screenWidth - 60, screenHeight - 60 };

            // Draw static compass ring
            DrawCircleV(compassCenter, compassRadius, LIGHTGRAY);
            DrawCircleLinesV(compassCenter, compassRadius, DARKGRAY);

            // Get the camera angle in radians (theta)
            float angle = -theta + PI / 2;

            // Triangle dimensions
            float pointerLength = compassRadius - 10;
            float baseWidth = 8.0f;

            // Calculate triangle points
            Vector2 northTip = {
                compassCenter.x + sinf(angle) * pointerLength,
                compassCenter.y - cosf(angle) * pointerLength
            };

            Vector2 southTip = {
                compassCenter.x - sinf(angle) * pointerLength,
                compassCenter.y + cosf(angle) * pointerLength
            };

            Vector2 left = {
                compassCenter.x + sinf(angle + PI * 0.5f) * (baseWidth / 2),
                compassCenter.y - cosf(angle + PI * 0.5f) * (baseWidth / 2)
            };

            Vector2 right = {
                compassCenter.x + sinf(angle - PI * 0.5f) * (baseWidth / 2),
                compassCenter.y - cosf(angle - PI * 0.5f) * (baseWidth / 2)
            };

            // Draw triangle pointing north & south
            DrawTriangle(northTip, right, left, RED);
            DrawTriangle(southTip, left, right, WHITE);

            // Draw rotating cardinal labels
            const char* labels[] = { "N", "E", "S", "W" };

            for (int i = 0; i < 4; i++)
            {
                float labelAngle = angle + i * (PI / 2.0f);
                float lx = sinf(labelAngle) * (compassRadius + 5);
                float ly = -cosf(labelAngle) * (compassRadius + 5);
                Vector2 labelPos = { compassCenter.x + lx, compassCenter.y + ly };
            
                const char* label = labels[i];
                int textWidth = MeasureText(label, 10);
                DrawText(label, labelPos.x - textWidth / 2, labelPos.y - 5, 10, BLACK);
            }

            // Draw left sidebar
            DrawRectangle(0, 0, sidebarWidth, screenHeight, LIGHTGRAY);

            switch (state)
            {
                case State::Configuring:
                    DrawText("Configuration", 20, 20, 20, DARKGRAY);

                    GuiLabel(Rectangle{ 20, 60, sidebarWidth - 40, 40 }, filename.c_str());

                    if (GuiButton(Rectangle{ 20, 110, sidebarWidth - 40, 40 }, "Open Configuration"))
                    {
                        fileDialog.saveFileMode = false;
                        fileDialog.windowActive = true;
                    }

                    GuiLabel(Rectangle{ 20, 170, sidebarWidth - 40, 40 }, "Select An Algorithm!");

                    if (GuiDropdownBox(Rectangle{ 20, 220, sidebarWidth - 40, 40 }, "Uniform Cost Search;Greedy Best First Search;A* (A-Star)", &selectedAlgorithm, dropdownActive))
                    {
                        dropdownActive = !dropdownActive;
                    }

                    if (GuiButton(Rectangle{ 20, screenHeight - 80, sidebarWidth - 40, 40 }, "Solve"))
                    {
                        if (board != nullptr)
                        {
                            CumulativeZeroCost cumZero;
                            CumulativeMoveCost cumMove;

                            EstimatedZeroCost estZero;
                            BlockingCarsCost estBlock;

                            Solver solver;
                            solver.SetBoard(*board);

                            switch (selectedAlgorithm)
                            {
                                case 0:
                                    solver.SetBacktracking(true);
                                    solver.SetCumulativeHeuristic(cumMove);
                                    solver.SetEstimatedHeuristic(estZero);
                                break;
                                case 1:
                                    solver.SetBacktracking(false);
                                    solver.SetCumulativeHeuristic(cumZero);
                                    solver.SetEstimatedHeuristic(estBlock);
                                break;
                                case 2:
                                    solver.SetBacktracking(true);
                                    solver.SetCumulativeHeuristic(cumMove);
                                    solver.SetEstimatedHeuristic(estBlock);
                                break;
                                default:
                                    EndDrawing();
                                    UnloadRenderTexture(screenCamera);
                                    CloseWindow();
                                return 1;
                            }

                            fileDialog.windowActive = false;

                            DrawTextureRec(screenCamera.texture, cameraRect, (Vector2){ sidebarWidth, 0 }, Color{ 16, 16, 16, 128 });
                            DrawText("Loading...", 30, screenHeight - 30, 20, WHITE);

                            EndDrawing();

                            solution = new Solution(solver.Solve());
                            state = State::ShowingResult;
                            moveIndex = -1;

                            continue;
                        }
                    }
                break;
                case State::ShowingResult:
                    DrawText("Solution", 20, 20, 20, DARKGRAY);

                    GuiLabel(Rectangle{ 20, 60, sidebarWidth - 40, 40 }, "Actions Taken:");

                    if (moveIndex + 1 < static_cast<int>(solution->moves.size()))
                    {
                        GuiCheckBox(Rectangle{ 20, screenHeight - 130, 40, 40 }, "Auto", &autoEnabled);
                        
                        if (autoEnabled)
                        {
                            timer += GetFrameTime();
                        }
                        else
                        {
                            timer = 0.9f;
                        }

                        if (GuiButton(Rectangle{ 20, screenHeight - 80, sidebarWidth - 40, 40 }, "Next") || timer >= 1.0f)
                        {
                            timer = 0.0f;

                            if (moveIndex >= 0)
                            {
                                int pieceIndex = solution->moves[moveIndex].piece;
                                Piece piece = board->GetPiece(pieceIndex);

                                Car& car = cars.at(pieceIndex);
                                car.lerp = 1;
                                car.xPast = car.xCurrent;
                                car.yPast = car.yCurrent;
                            }

                            moveIndex++;

                            Move move = solution->moves[moveIndex];
                            int pieceIndex = move.piece;
                            Piece piece = board->GetPiece(pieceIndex);

                            Car& car = cars.at(pieceIndex);
                            car.lerp = 0;

                            std::string label = std::string(1, carLabels.at(pieceIndex)) + " : ";
                            switch (piece.GetOrientation())
                            {
                                case Piece::Orientation::Horizontal:
                                    if (move.offset < 0)
                                    {
                                        label += std::to_string(-move.offset) + " West";
                                    }
                                    else
                                    {
                                        label += std::to_string(move.offset) + " East";
                                    }


                                    car.xCurrent += solution->moves[moveIndex].offset;
                                break;
                                case Piece::Orientation::Vertical:
                                    if (move.offset < 0)
                                    {
                                        label += std::to_string(-move.offset) + " North";
                                    }
                                    else
                                    {
                                        label += std::to_string(move.offset) + " South";
                                    }

                                    car.yCurrent += solution->moves[moveIndex].offset;
                                break;
                            }
                            char* str = new char[label.size() + 1];
                            strncpy(str, label.c_str(), label.size() + 1);
                            moveLabels.push_back(str);
                        }
                    }
                    else
                    {
                        autoEnabled = false;
                        timer = 0.0f;

                        if (GuiButton(Rectangle{ 20, screenHeight - 130, sidebarWidth - 40, 40 }, "Save"))
                        {
                            fileDialog.saveFileMode = true;
                            fileDialog.windowActive = true;
                        }

                        if (GuiButton(Rectangle{ 20, screenHeight - 80, sidebarWidth - 40, 40 }, "Return"))
                        {
                            fileDialog.windowActive = false;

                            delete solution;
                            solution = nullptr;
                            cars.clear();
                            carLabels.clear();

                            delete board;
                            board = nullptr;
                            filename = "No File Selected";

                            for (size_t i = 0; i < moveLabels.size(); i++)
                            {
                                delete[] moveLabels[i];
                            }

                            moveLabels.clear();
                            
                            autoEnabled = false;

                            state = State::Configuring;

                            EndDrawing();

                            continue;
                        }
                    }

                    if (solution->moves.size() > 0)
                    {
                        GuiListViewEx(Rectangle{ 20, 110, sidebarWidth - 40, screenHeight - 300 }, moveLabels.data(), moveLabels.size(), &moveScrollIndex, &moveListActive, &moveListFocus);

                        if (moveListFocus >= 0)
                        {
                            highlightedCar = solution->moves[moveListFocus].piece;
                        }
                        else
                        {
                            highlightedCar = -1;
                        }
                    }
                    else
                    {
                        GuiLabel(Rectangle{ 20, 110, sidebarWidth - 40, 40 }, "None");
                    }
                break;
                default:
                    assert(false);
            }

            GuiWindowFileDialog(&fileDialog);

        EndDrawing();
    }

    UnloadRenderTexture(screenCamera);

    CloseWindow();
    return 0;
}
