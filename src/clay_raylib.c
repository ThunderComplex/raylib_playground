#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_raylib_renderer.c"
#include "raylib.h"
#include "raymath.h"

void HandleClayErrors(Clay_ErrorData errorData)
{
    printf("%s\n", errorData.errorText.chars);
}

// Example measure text function
static inline Clay_Dimensions MeasureTextClay(Clay_StringSlice text, Clay_TextElementConfig* config, uintptr_t userData)
{
    // Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
    // Note: Clay_String->chars is not guaranteed to be null terminated
    return (Clay_Dimensions) {
        .width = text.length * 12, // <- this will only work for monospace fonts, see the renderers/ directory for more advanced text measurement
        .height = config->fontSize
    };
}

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 800;

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    Clay_Dimensions dimensions = { .width = screenWidth, .height = screenHeight };
    Clay_Initialize(clayMemory, dimensions, (Clay_ErrorHandler) { HandleClayErrors });
    Clay_Raylib_Initialize(
        screenWidth,
        screenHeight,
        "raylib + clay",
        FLAG_WINDOW_RESIZABLE);

    Color bgColor = ColorLerp(DARKBLUE, BLACK, 0.69f);

    SetTargetFPS(60);
    Font defaultFont = GetFontDefault();
    Clay_SetMeasureTextFunction(MeasureTextClay, &defaultFont);

    while (!WindowShouldClose()) {
        Clay_BeginLayout();

        CLAY(CLAY_ID("OuterContainer"), { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16 }, .backgroundColor = { 250, 250, 200, 255 } })
        {
            CLAY(CLAY_ID("SideBar"), { .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16 }, .backgroundColor = { 255, 64, 64, 255 } })
            {
                CLAY(CLAY_ID("ProfilePictureOuter"), { .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = { 64, 200, 64, 255 } })
                {
                    CLAY(CLAY_ID("ProfilePicture"), { .layout = { .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) } } }) { }
                    CLAY_TEXT(CLAY_STRING("Clay UI Library"), CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = { 255, 255, 255, 255 } }));
                }

                CLAY(CLAY_ID("MainContent"), { .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } }, .backgroundColor = { 255, 64, 64 } }) { }
            }
        }
        Clay_RenderCommandArray renderCommands = Clay_EndLayout();

        BeginDrawing();

        ClearBackground(bgColor);

        Clay_Raylib_Render(renderCommands, &defaultFont);

        EndDrawing();
    }

    Clay_Raylib_Close();

    return 0;
}
