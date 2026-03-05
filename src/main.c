#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define PLAYER_SIZE 80
#define INVENTORY_SIZE 16
#define WORLD_ITEM_COUNT 256
#define ITEM_WORLD_SIZE 80

typedef enum {
    ITEM_TYPE_NONE = 1,
    ITEM_TYPE_EQUIPPABLE,
    ITEM_TYPE_USABLE,
    ITEM_TYPE_GUN
} ItemType;

typedef struct Item {
    uint64_t id;
    char* name;
    ItemType itemType;
    Rectangle world_position;
} Item;

static Item Inventory[INVENTORY_SIZE];
static Item WorldItems[WORLD_ITEM_COUNT];

void DrawInventory(int offsetX, int width, int height)
{
    DrawRectangle(offsetX + 0, 0, width, height / 2, (Color) { 64, 64, 64, 255 });
    DrawRectangle(offsetX + 0, height / 2, width, height / 2, (Color) { 32, 32, 40, 255 });

    int margin_x = 20;
    int margin_y = 20;
    float button_width = 100;
    float button_height = 50;
    float button_x = offsetX + margin_x;
    float button_y = margin_y;

    for (size_t i = 0; i < INVENTORY_SIZE; i++)
    {
        Item inventoryItem = Inventory[i];

        if (inventoryItem.id == 0) {
            continue;
        }
        
        // float button_x = offsetX + margin_x + (((margin_x + (int)ceil(button_width)) * i) % (width - margin_x - (int)ceil(button_width)));
        // float button_x = offsetX + margin_x + (((button_width + margin_x) * i) / (width + margin_x + button_width));    
        // float button_y = (button_height + margin_y) * floor((offsetX + margin_x + (((margin_x + ceil(button_width)) * (i + 1)))) / width);
        // float button_offset_x = (offsetX + ((int)button_width * (i + 1))) % width;
        // float button_offset_y = (((offsetX + ((int)button_width * i)) / width) - 1) * (20 + button_height);

        if (GuiButton((Rectangle) { button_x, button_y, button_width, button_height }, inventoryItem.name))
        {
             printf("Item: %d -> %s\n", inventoryItem.id, inventoryItem.name);
             printf("x: %f, y: %f\n", button_x, button_y);
        }

        button_x += button_width + margin_x;

        if (button_x + margin_x >= offsetX + width - button_width) {
            button_x = offsetX + margin_x;
            button_y += button_height + margin_y;
        }
    }
}

bool AddInventoryItem(Item item)
{
    for (size_t i = 0; i < INVENTORY_SIZE; i++)
    {
        Item inventoryItem = Inventory[i];

        if (inventoryItem.id != 0) {
            continue;
        }

        Inventory[i] = item;
        return true;
    }

    return false;
}

void SetupInitialItems()
{
    for (int i = 1; i < 16; i++)
    {
        char* buffer = calloc(10, sizeof(char));

        if (buffer == NULL) {
            printf("Unrecoverable error!");
            exit(-1);
        }

        sprintf_s(buffer, 10, "Item %d", i);

        WorldItems[i] = (Item){
            .id = i,
            .name = buffer,
            .itemType = ITEM_TYPE_EQUIPPABLE,
            .world_position = (Rectangle) {i * 2 * ITEM_WORLD_SIZE, 80, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE},
        };
        // free(buffer);
    }
}

void DrawWorldItems()
{
    for (size_t i = 0; i < WORLD_ITEM_COUNT; i++)
    {
        Item item = WorldItems[i];

        if (item.id == 0) {
            continue;
        }

        DrawRectangleRec(item.world_position, GREEN);
    }
}

void PlayerWorldItemCollision(Rectangle player)
{
    for (size_t i = 0; i < WORLD_ITEM_COUNT; i++)
    {
        Item item = WorldItems[i];

        if (item.id == 0) {
            continue;
        }

        if (CheckCollisionRecs(player, item.world_position)) {
            AddInventoryItem(item);
            WorldItems[i].id = 0;
        }
    }
}

int main(void)
{
    int screenWidth = 1200;
    int screenHeight = 1000;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Game");

    Rectangle player = { 240, 240, PLAYER_SIZE, PLAYER_SIZE };

    Camera2D camera1 = { 0 };
    camera1.target = (Vector2) { player.x, player.y };
    camera1.offset = (Vector2) { 0.0f, 0.0f };
    camera1.rotation = 0.0f;
    camera1.zoom = 1.0f;

    RenderTexture screenCamera1 = LoadRenderTexture(screenWidth / 2, screenHeight);

    Rectangle splitScreenRect = { 0.0f, 0.0f, (float)screenCamera1.texture.width, (float)-screenCamera1.texture.height };

    SetTargetFPS(60);

    SetupInitialItems();

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            UnloadRenderTexture(screenCamera1);
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
            screenCamera1 = LoadRenderTexture(screenWidth / 2, screenHeight);
            splitScreenRect = (Rectangle) { 0.0f, 0.0f, (float)screenCamera1.texture.width, (float)-screenCamera1.texture.height };
        }

        //
        // INPUT
        //

        if (IsKeyPressed(KEY_S))
            player.y += PLAYER_SIZE;
        else if (IsKeyPressed(KEY_W))
            player.y -= PLAYER_SIZE;
        if (IsKeyPressed(KEY_D))
            player.x += PLAYER_SIZE;
        else if (IsKeyPressed(KEY_A))
            player.x -= PLAYER_SIZE;


        camera1.target = (Vector2) { player.x, player.y };
        camera1.offset = (Vector2) { splitScreenRect.width / 2, -splitScreenRect.height / 2 };

        //
        // PHYSICS
        //

        PlayerWorldItemCollision(player);

        //
        // WORLD RENDERING
        //

        BeginTextureMode(screenCamera1);
        ClearBackground(RAYWHITE);

        BeginMode2D(camera1);

        for (int i = 0; i < screenWidth / PLAYER_SIZE + 1; i++) {
            DrawLineV((Vector2) { (float)PLAYER_SIZE * i, 0 }, (Vector2) { (float)PLAYER_SIZE * i, (float)screenHeight }, LIGHTGRAY);
        }

        for (int i = 0; i < screenHeight / PLAYER_SIZE + 1; i++) {
            DrawLineV((Vector2) { 0, (float)PLAYER_SIZE * i }, (Vector2) { (float)screenWidth, (float)PLAYER_SIZE * i }, LIGHTGRAY);
        }

        for (int i = 0; i < screenWidth / PLAYER_SIZE; i++) {
            for (int j = 0; j < screenHeight / PLAYER_SIZE; j++) {
                DrawText(TextFormat("[%i,%i]", i, j), 10 + PLAYER_SIZE * i, 15 + PLAYER_SIZE * j, 10, LIGHTGRAY);
            }
        }

        DrawWorldItems();

        DrawRectangleRec(player, RED);

        EndMode2D();
        EndTextureMode();

        //
        // INVENTORY RENDERING
        //

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextureRec(screenCamera1.texture, splitScreenRect, (Vector2) { 0, 0 }, WHITE);

        DrawInventory(screenWidth / 2, screenWidth / 2, screenHeight);

        DrawRectangle(screenWidth / 2 - 2, 0, 4, screenHeight, (Color) {12, 24, 12, 255});
        
        EndDrawing();
    }

    UnloadRenderTexture(screenCamera1);

    CloseWindow();

    return 0;
}
