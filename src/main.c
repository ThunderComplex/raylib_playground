#include "raylib.h"

#define PLAYER_SIZE 80

int main(void)
{
    int screenWidth = 1200;
    int screenHeight = 1000;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Game");

    Rectangle player1 = { 200, 200, PLAYER_SIZE, PLAYER_SIZE };
    Rectangle player2 = { 250, 200, PLAYER_SIZE, PLAYER_SIZE };

    Camera2D camera1 = { 0 };
    camera1.target = (Vector2){ player1.x, player1.y };
    camera1.offset = (Vector2){ 0.0f, 0.0f };
    camera1.rotation = 0.0f;
    camera1.zoom = 1.0f;

    Camera2D camera2 = { 0 };
    camera2.target = (Vector2){ player2.x, player2.y };
    camera2.offset = (Vector2){ 0.0f, 0.0f };
    camera2.rotation = 0.0f;
    camera2.zoom = 1.0f;

    RenderTexture screenCamera1 = LoadRenderTexture(screenWidth / 2, screenHeight);
    RenderTexture screenCamera2 = LoadRenderTexture(screenWidth / 2, screenHeight);

    // Build a flipped rectangle the size of the split view to use for drawing later
    Rectangle splitScreenRect = { 0.0f, 0.0f, (float)screenCamera1.texture.width, (float)-screenCamera1.texture.height };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            UnloadRenderTexture(screenCamera1);
            UnloadRenderTexture(screenCamera2);
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
            screenCamera1 = LoadRenderTexture(screenWidth / 2, screenHeight);
            screenCamera2 = LoadRenderTexture(screenWidth / 2, screenHeight);
            splitScreenRect = (Rectangle){ 0.0f, 0.0f, (float)screenCamera1.texture.width, (float)-screenCamera1.texture.height };
        }

        if (IsKeyDown(KEY_S))
            player1.y += 3.0f;
        else if (IsKeyDown(KEY_W))
            player1.y -= 3.0f;
        if (IsKeyDown(KEY_D))
            player1.x += 3.0f;
        else if (IsKeyDown(KEY_A))
            player1.x -= 3.0f;

        if (IsKeyDown(KEY_UP))
            player2.y -= 3.0f;
        else if (IsKeyDown(KEY_DOWN))
            player2.y += 3.0f;
        if (IsKeyDown(KEY_RIGHT))
            player2.x += 3.0f;
        else if (IsKeyDown(KEY_LEFT))
            player2.x -= 3.0f;

        camera1.target = (Vector2){ player1.x, player1.y };
        camera1.offset = (Vector2){ splitScreenRect.width / 2, -splitScreenRect.height / 2 };
        camera2.target = (Vector2){ player2.x, player2.y };
        camera2.offset = (Vector2){ splitScreenRect.width / 2, -splitScreenRect.height / 2 };

        BeginTextureMode(screenCamera1);
        ClearBackground(RAYWHITE);

        BeginMode2D(camera1);

        // Draw full scene with first camera
        for (int i = 0; i < screenWidth / PLAYER_SIZE + 1; i++) {
            DrawLineV((Vector2) { (float)PLAYER_SIZE* i, 0 }, (Vector2) { (float)PLAYER_SIZE* i, (float)screenHeight }, LIGHTGRAY);
        }

        for (int i = 0; i < screenHeight / PLAYER_SIZE + 1; i++) {
            DrawLineV((Vector2) { 0, (float)PLAYER_SIZE* i }, (Vector2) { (float)screenWidth, (float)PLAYER_SIZE* i }, LIGHTGRAY);
        }

        for (int i = 0; i < screenWidth / PLAYER_SIZE; i++) {
            for (int j = 0; j < screenHeight / PLAYER_SIZE; j++) {
                DrawText(TextFormat("[%i,%i]", i, j), 10 + PLAYER_SIZE * i, 15 + PLAYER_SIZE * j, 10, LIGHTGRAY);
            }
        }

        DrawRectangleRec(player1, RED);
        DrawRectangleRec(player2, BLUE);
        EndMode2D();

        DrawRectangle(0, 0, screenWidth / 2, 30, Fade(RAYWHITE, 0.6f));
        DrawText("PLAYER1: W/S/A/D to move", 10, 10, 10, MAROON);

        EndTextureMode();

        BeginTextureMode(screenCamera2);
        ClearBackground(RAYWHITE);

        BeginMode2D(camera2);

        // Draw full scene with second camera
        for (int i = 0; i < screenWidth / PLAYER_SIZE + 1; i++) {
            DrawLineV((Vector2) { (float)PLAYER_SIZE* i, 0 }, (Vector2) { (float)PLAYER_SIZE* i, (float)screenHeight }, LIGHTGRAY);
        }

        for (int i = 0; i < screenHeight / PLAYER_SIZE + 1; i++) {
            DrawLineV((Vector2) { 0, (float)PLAYER_SIZE* i }, (Vector2) { (float)screenWidth, (float)PLAYER_SIZE* i }, LIGHTGRAY);
        }

        for (int i = 0; i < screenWidth / PLAYER_SIZE; i++) {
            for (int j = 0; j < screenHeight / PLAYER_SIZE; j++) {
                DrawText(TextFormat("[%i,%i]", i, j), 10 + PLAYER_SIZE * i, 15 + PLAYER_SIZE * j, 10, LIGHTGRAY);
            }
        }

        DrawRectangleRec(player1, RED);
        DrawRectangleRec(player2, BLUE);

        EndMode2D();

        DrawRectangle(0, 0, screenWidth / 2, 30, Fade(RAYWHITE, 0.6f));
        DrawText("PLAYER2: UP/DOWN/LEFT/RIGHT to move", 10, 10, 10, DARKBLUE);

        EndTextureMode();

        // Draw both views render textures to the screen side by side
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextureRec(screenCamera1.texture, splitScreenRect, (Vector2) { 0, 0 }, WHITE);
        DrawTextureRec(screenCamera2.texture, splitScreenRect, (Vector2) { screenWidth / 2.0f, 0 }, WHITE);

        DrawRectangle(screenWidth / 2 - 2, 0, 4, screenHeight, LIGHTGRAY);
        EndDrawing();
    }

    UnloadRenderTexture(screenCamera1);
    UnloadRenderTexture(screenCamera2);

    CloseWindow();

    return 0;
}
