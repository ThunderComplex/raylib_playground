#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#define GRID_SIZE 80
#define INVENTORY_SIZE 4
#define WORLD_ITEM_COUNT 256
#define ITEM_WORLD_SIZE 80
#define ITEM_DEFS_COUNT 64
#define MAX_EQUIPPED_ITEMS 10

typedef enum {
    ITEM_TYPE_NONE = 1,
    ITEM_TYPE_AMMO,
    ITEM_TYPE_EQUIPPABLE,
    ITEM_TYPE_USABLE,
    ITEM_TYPE_GUN
} ItemType;

typedef struct Container {
    uint64_t id;
    char name[256];
} Container;

typedef struct Item {
    uint64_t id;
    char name[256];
    ItemType item_type;
    Container* parent_container;
    Rectangle world_position;
    int stack_size;
    int max_stack;
} Item;

static Item ItemDefs[ITEM_DEFS_COUNT];
static Item Inventory[INVENTORY_SIZE];
static Item WorldItems[WORLD_ITEM_COUNT];
static Item EquippedItems[MAX_EQUIPPED_ITEMS];

static Font TextFont;

void* calloc_or_die(size_t count, size_t size)
{
    void* data = calloc(count, size);

    if (data == 0)
    {
        printf("Could not allocate memory, recovery impossible. Exiting...");
        CloseWindow();
        exit(-1);
    }

    return data;
}

void RemoveInventoryItem(int id)
{
    Inventory[id].id = 0;
    return;

    /*for (size_t i = 0; i < INVENTORY_SIZE; i++)
    {
        Item inventoryItem = Inventory[i];

        if (inventoryItem.id != 0) {
            continue;
        }

        Inventory[i] = item;
    }*/
}

bool AddInventoryItem(Item item)
{
    for (size_t i = 0; i < INVENTORY_SIZE; i++)
    {
        Item inventoryItem = Inventory[i];

        // Adding existing item?
        if (inventoryItem.id == item.id)
        {
            if (inventoryItem.max_stack >= inventoryItem.stack_size + item.stack_size)
            {
                Inventory[i].stack_size += item.stack_size;

                return true;
            }
            else
            {
                return false;
            }
        }
        // Not empty
        else if (inventoryItem.id != 0)
        {
            continue;
        }

        Inventory[i] = item;
        return true;
    }

    return false;
}

bool EquipItem(Item item)
{
    for (size_t i = 0; i < MAX_EQUIPPED_ITEMS; i++)
    {
        Item equippedItem = EquippedItems[i];

        if (equippedItem.id != 0)
        {
            continue;
        }

        EquippedItems[i] = item;
        return true;
    }

    return false;
}

void UnequipItem(int id)
{
    EquippedItems[id].id = 0;
}

void DrawInventory(int offsetX, int width, int height)
{
    DrawRectangle(offsetX + 0, 0, width, height / 2, (Color) { 64, 64, 64, 255 });

    int margin_x = 20;
    int margin_y = 20;
    float button_width = 100;
    float button_height = 50;
    float button_x = offsetX + margin_x;
    float button_y = margin_y;

    for (size_t i = 0; i < INVENTORY_SIZE; i++)
    {
        Item inventoryItem = Inventory[i];

        if (inventoryItem.id == 0)
        {
            continue;
        }

        if (GuiButton((Rectangle) { button_x, button_y, button_width, button_height }, inventoryItem.name))
        {
            if (inventoryItem.item_type == ITEM_TYPE_GUN)
            {
                RemoveInventoryItem(i);
                EquipItem(inventoryItem);
            }
            //RemoveInventoryItem(i);
        }

        const char* stack_size_text = calloc_or_die(256, sizeof(char));
        sprintf_s(stack_size_text, 256, "%d", inventoryItem.stack_size);
        DrawTextEx(TextFont, stack_size_text, (Vector2) { button_x + 10, button_y + 25 }, 30, 1.2, RED);
        free(stack_size_text);

        button_x += button_width + margin_x;

        if (button_x + margin_x >= offsetX + width - button_width) {
            button_x = offsetX + margin_x;
            button_y += button_height + margin_y;
        }
    }
}

void DrawEquipment(int offsetX, int width, int height)
{
    DrawRectangle(offsetX + 0, height / 2, width, height / 2, (Color) { 32, 32, 40, 255 });

    int margin_x = 20;
    int margin_y = 20;
    float button_width = 100;
    float button_height = 50;
    float button_x = offsetX + margin_x;
    float button_y = (height / 2) + margin_y;

    for (size_t i = 0; i < MAX_EQUIPPED_ITEMS; i++)
    {
        Item equippedItem = EquippedItems[i];

        if (equippedItem.id == 0)
        {
            continue;
        }

        if (GuiButton((Rectangle) { button_x, button_y, button_width, button_height }, equippedItem.name))
        {
            printf("Pew\n");
        }

        button_x += button_width + margin_x;

        if (button_x + margin_x >= offsetX + width - button_width) {
            button_x = offsetX + margin_x;
            button_y += button_height + margin_y;
        }
    }
}

void SetupInitialItems()
{
    ItemDefs[0] = (Item){
        .id = 1,
        .name = "SMG",
        .item_type = ITEM_TYPE_GUN,
        .world_position = (Rectangle) {2, 0, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE},
        .stack_size = 1,
        .max_stack = 1,
        .parent_container = NULL,
    };

    ItemDefs[1] = (Item){
        .id = 2,
        .name = "SMG Ammo",
        .item_type = ITEM_TYPE_AMMO,
        .world_position = (Rectangle) {4, 0, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE},
        .stack_size = 4,
        .max_stack = 16,
        .parent_container = NULL,
    };
}

void SpawnItemsInWorld()
{
    WorldItems[0] = ItemDefs[0];
    WorldItems[0].world_position = (Rectangle){ 2 * ITEM_WORLD_SIZE, 2 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[1] = ItemDefs[1];
    WorldItems[1].world_position = (Rectangle){ 4 * ITEM_WORLD_SIZE, 2 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[2] = ItemDefs[1];
    WorldItems[2].world_position = (Rectangle){ 6 * ITEM_WORLD_SIZE, 2 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[3] = ItemDefs[1];
    WorldItems[3].world_position = (Rectangle){ 8 * ITEM_WORLD_SIZE, 2 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[4] = ItemDefs[1];
    WorldItems[4].world_position = (Rectangle){ 10 * ITEM_WORLD_SIZE, 2 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[5] = ItemDefs[0];
    WorldItems[5].world_position = (Rectangle){ 2 * ITEM_WORLD_SIZE, 4 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[6] = ItemDefs[0];
    WorldItems[6].world_position = (Rectangle){ 2 * ITEM_WORLD_SIZE, 6 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[7] = ItemDefs[0];
    WorldItems[7].world_position = (Rectangle){ 2 * ITEM_WORLD_SIZE, 8 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[8] = ItemDefs[0];
    WorldItems[8].world_position = (Rectangle){ 2 * ITEM_WORLD_SIZE, 10 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };

    WorldItems[9] = ItemDefs[0];
    WorldItems[9].world_position = (Rectangle){ 2 * ITEM_WORLD_SIZE, 12 * ITEM_WORLD_SIZE, ITEM_WORLD_SIZE, ITEM_WORLD_SIZE };
}

void DrawWorldItems()
{
    for (size_t i = 0; i < WORLD_ITEM_COUNT; i++)
    {
        Item item = WorldItems[i];

        if (item.id == 0)
        {
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

        if (item.id == 0)
        {
            continue;
        }

        if (CheckCollisionRecs(player, item.world_position))
        {
            if (AddInventoryItem(item))
            {
                WorldItems[i].id = 0;
            }
        }
    }
}

int main(void)
{
    int screenWidth = 1200;
    int screenHeight = 1000;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Game");

    Rectangle player = { 240, 240, GRID_SIZE, GRID_SIZE };

    Camera2D camera1 = { 0 };
    camera1.target = (Vector2) { player.x, player.y };
    camera1.offset = (Vector2) { 0.0f, 0.0f };
    camera1.rotation = 0.0f;
    camera1.zoom = 1.0f;

    RenderTexture screenCamera1 = LoadRenderTexture(screenWidth / 2, screenHeight);

    Rectangle splitScreenRect = { 0.0f, 0.0f, (float)screenCamera1.texture.width, (float)-screenCamera1.texture.height };

    SetTargetFPS(60);

    TextFont = LoadFont("data/font/Rushfordclean.otf");

    if (!IsFontValid(TextFont)) {
        printf("Font could not be loaded\n");
        CloseWindow();
        exit(-2);
    }

    GuiSetFont(TextFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    /*GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_NONE);
    GuiSetStyle(DEFAULT, TEXT_LINE_SPACING, 10);*/

    SetupInitialItems();
    SpawnItemsInWorld();

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
            player.y += GRID_SIZE;
        else if (IsKeyPressed(KEY_W))
            player.y -= GRID_SIZE;
        if (IsKeyPressed(KEY_D))
            player.x += GRID_SIZE;
        else if (IsKeyPressed(KEY_A))
            player.x -= GRID_SIZE;


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

        for (int i = 0; i < screenWidth / GRID_SIZE + 1; i++) {
            DrawLineV((Vector2) { (float)GRID_SIZE * i, 0 }, (Vector2) { (float)GRID_SIZE * i, (float)screenHeight }, LIGHTGRAY);
        }

        for (int i = 0; i < screenHeight / GRID_SIZE + 1; i++) {
            DrawLineV((Vector2) { 0, (float)GRID_SIZE * i }, (Vector2) { (float)screenWidth, (float)GRID_SIZE * i }, LIGHTGRAY);
        }

        for (int i = 0; i < screenWidth / GRID_SIZE; i++) {
            for (int j = 0; j < screenHeight / GRID_SIZE; j++) {
                DrawText(TextFormat("[%i,%i]", i, j), 10 + GRID_SIZE * i, 15 + GRID_SIZE * j, 10, LIGHTGRAY);
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
        DrawEquipment(screenWidth / 2, screenWidth / 2, screenHeight);

        DrawRectangle(screenWidth / 2 - 2, 0, 4, screenHeight, (Color) {12, 24, 12, 255});
        
        EndDrawing();
    }

    UnloadRenderTexture(screenCamera1);

    CloseWindow();

    return 0;
}
