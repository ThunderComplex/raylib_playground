/*******************************************************************************************
 *
 *   raylib [core] example - random sequence
 *
 *   Example complexity rating: [★☆☆☆] 1/4
 *
 *   Example originally created with raylib 5.0, last time updated with raylib 5.0
 *
 *   Example contributed by Dalton Overmyer (@REDl3east) and reviewed by Ramon Santamaria (@raysan5)
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2023-2025 Dalton Overmyer (@REDl3east)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include <stdlib.h> // Required for: malloc(), free()

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct ColorRect {
    Color color;
    Rectangle rect;
} ColorRect;

//------------------------------------------------------------------------------------
// Module Functions Declaration
//------------------------------------------------------------------------------------
static Color GenerateRandomColor(void);
static ColorRect* GenerateRandomColorRectSequence(float rectCount, float rectWidth, float screenWidth, float screenHeight);
static void ShuffleColorRectSequence(ColorRect* rectangles, int rectCount);
static void SortRects(ColorRect* rects, int rectCount);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 1200;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - random sequence");

    int rectCount = 20;
    float rectSize = (float)screenWidth / rectCount;
    ColorRect* rectangles = GenerateRandomColorRectSequence((float)rectCount, rectSize, (float)screenWidth, 0.75f * screenHeight);

    SetTargetFPS(144);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_SPACE))
            SortRects(rectangles, rectCount);
        // ShuffleColorRectSequence(rectangles, rectCount);

        if (IsKeyDown(KEY_UP)) {
            rectCount+=10;
            rectSize = (float)screenWidth / rectCount;
            RL_FREE(rectangles);

            // Re-generate random sequence with new count
            rectangles = GenerateRandomColorRectSequence((float)rectCount, rectSize, (float)screenWidth, 0.75f * screenHeight);
        }

        if (IsKeyDown(KEY_DOWN)) {
            if (rectCount >= 4) {
                rectCount--;
                rectSize = (float)screenWidth / rectCount;
                RL_FREE(rectangles);

                // Re-generate random sequence with new count
                rectangles = GenerateRandomColorRectSequence((float)rectCount, rectSize, (float)screenWidth, 0.75f * screenHeight);
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < rectCount; i++) {
            // Vector2 topLeft = { 0 };
            // Vector2 topRight = { 0 };
            // Vector2 bottomLeft = { 0 };
            // Vector2 bottomRight = { 0 };
            // Rectangle rec = rectangles[i].rect;
            // Vector2 origin = { 0 };
            // float x = rec.x - origin.x;
            // float y = rec.y - origin.y;
            // topLeft = (Vector2) { x, y };
            // topRight = (Vector2) { x + rec.width, y };
            // bottomLeft = (Vector2) { x, y + rec.height };
            // bottomRight = (Vector2) { x + rec.width, y + rec.height };
            // rlBegin(RL_TRIANGLES);

            // rlColor4ub(rectangles[i].color.r, rectangles[i].color.g, rectangles[i].color.b, rectangles[i].color.a);

            // rlVertex2f(topLeft.x, topLeft.y);
            // rlVertex2f(bottomLeft.x, bottomLeft.y);
            // rlVertex2f(topRight.x, topRight.y);

            // rlVertex2f(topRight.x, topRight.y);
            // rlVertex2f(bottomLeft.x, bottomLeft.y);
            // rlVertex2f(bottomRight.x, bottomRight.y);

            // rlEnd();

            DrawRectangleRec(rectangles[i].rect, rectangles[i].color);

            DrawText("Press SPACE to shuffle the current sequence", 10, screenHeight - 96, 20, BLACK);
            DrawText("Press UP to add a rectangle and generate a new sequence", 10, screenHeight - 64, 20, BLACK);
            DrawText("Press DOWN to remove a rectangle and generate a new sequence", 10, screenHeight - 32, 20, BLACK);
        }

        DrawText(TextFormat("Count: %d rectangles", rectCount), 10, 10, 20, MAROON);

        DrawFPS(screenWidth - 80, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    free(rectangles);
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definition
//------------------------------------------------------------------------------------
static Color GenerateRandomColor(void)
{
    Color color = {
        GetRandomValue(0, 255),
        GetRandomValue(0, 255),
        GetRandomValue(0, 255),
        255
    };

    return color;
}

static ColorRect* GenerateRandomColorRectSequence(float rectCount, float rectWidth, float screenWidth, float screenHeight)
{
    ColorRect* rectangles = (ColorRect*)RL_CALLOC((int)rectCount, sizeof(ColorRect));

    int* seq = LoadRandomSequence((unsigned int)rectCount, 0, (unsigned int)rectCount - 1);
    float rectSeqWidth = rectCount * rectWidth;
    float startX = (screenWidth - rectSeqWidth) * 0.5f;

    for (int i = 0; i < rectCount; i++) {
        int rectHeight = (int)Remap((float)seq[i], 0, rectCount - 1, 0, screenHeight);

        rectangles[i].color = GenerateRandomColor();
        rectangles[i].rect = CLITERAL(Rectangle) { startX + i * rectWidth, screenHeight - rectHeight, rectWidth, (float)rectHeight };
    }

    UnloadRandomSequence(seq);

    return rectangles;
}

static void SwapRects(ColorRect* a, ColorRect* b)
{
    ColorRect tmp = *a;
    a->color = b->color;
    a->rect.height = b->rect.height;
    a->rect.y = b->rect.y;
    b->color = tmp.color;
    b->rect.height = tmp.rect.height;
    b->rect.y = tmp.rect.y;
}

static void ShuffleColorRectSequence(ColorRect* rectangles, int rectCount)
{
    int* seq = LoadRandomSequence(rectCount, 0, rectCount - 1);

    for (int i1 = 0; i1 < rectCount; i1++) {
        ColorRect* r1 = &rectangles[i1];
        ColorRect* r2 = &rectangles[seq[i1]];

        SwapRects(r1, r2);
    }

    UnloadRandomSequence(seq);
}

static void SortRects(ColorRect* rects, int rectCount)
{
    for (size_t j = 0; j < 10; j++) {
        for (int i = 0; i < rectCount - 1; ++i) {
            ColorRect* r1 = &rects[i];
            ColorRect* r2 = &rects[i + 1];

            if (r1->rect.height > r2->rect.height) {
                SwapRects(r1, r2);
            }
        }
    }
}
