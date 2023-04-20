#include <cstdlib>
#include <algorithm>
#include <string>
#include <cmath>
#include "raylib.h"

#include "config.h"

Vector2 vMouse{};
unsigned int frameCounter = 0;

Vector2 calcVMouse(float scale, Rectangle canvas);

void Draw9Slice(Texture2D &tex, Rectangle rec, float thickness, Color tint);


int main() {
    // Raylib initialization
    // Project name, screen size, fullscreen mode etc. can be specified in the config.h.in file
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(Game::ScreenWidth, Game::ScreenHeight, Game::PROJECT_NAME);
    SetTargetFPS(60);

#ifdef GAME_START_FULLSCREEN
    ToggleFullscreen();
#endif

    // Your own initialization code here
    // ...
    // ...
    Texture2D boxNeutral = LoadTexture("assets/graphics/boxNeutral.png");
    Texture2D boxHover = LoadTexture("assets/graphics/boxHover.png");
    Texture2D boxPress = LoadTexture("assets/graphics/boxPress.png");
    Texture2D bg = LoadTexture("assets/graphics/bg.png");
    RenderTexture2D canvas = LoadRenderTexture(Game::ScreenWidth, Game::ScreenHeight);
    float renderScale{}; //those two are relevant to drawing and code-cleanliness
    Rectangle renderRec{};
    float thickness = 16;
    float fontsize = 16;
    std::string myString = "Knopp'";
    Rectangle wobblyButtonOG = {768, 96, 48, 48};
    Rectangle buttons[6] = {
            {96,  32,  64,      64},
            {96,  160, 32 * 18, 64},
            {},
            {96,  256, 32,      320},
            {320, 288, 64,      64},
            {704, 256, 8 * 32,  8 * 32}
    };


    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)) { //Fullscreen logic.
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(Game::ScreenWidth, Game::ScreenHeight);
            } else {
                SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
                ToggleFullscreen();
            }
        }

        // Updates that are made by frame are coded here
        // ...
        // ...
        vMouse = calcVMouse(renderScale, renderRec);
        BeginDrawing();
        // You can draw on the screen between BeginDrawing() and EndDrawing()
        // For the letterbox we draw on canvas instad
        BeginTextureMode(canvas);
        { //Within this block is where we draw our app to the canvas.
            if (IsKeyPressed(KEY_M)){
                SetWindowSize(Game::ScreenWidth*2,Game::ScreenHeight*2+32);
            }
            ClearBackground(WHITE);
            float offisetti = sin(frameCounter / 50.f) * 16 + 16;
            buttons[2].width = wobblyButtonOG.width + offisetti;
            buttons[2].height = wobblyButtonOG.height + offisetti;
            buttons[2].x = wobblyButtonOG.x - offisetti/2.0f;
            buttons[2].y = wobblyButtonOG.y - offisetti/2.0f;
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                if (IsKeyDown(KEY_DOWN)) {
                    fontsize--;
                }
                if (IsKeyDown(KEY_UP)) {
                    fontsize++;
                }
                if (IsKeyPressed(KEY_RIGHT)) {
                    fontsize = 16;
                }
            } else {
                if (IsKeyDown(KEY_DOWN)) {
                    thickness--;
                }
                if (IsKeyDown(KEY_UP)) {
                    thickness++;
                }
                if (IsKeyPressed(KEY_RIGHT)) {
                    thickness = 16;
                }
            }
            //draw Buttons
            for (auto i = 5; i >= 0; i--) {
                if (CheckCollisionPointRec(vMouse, buttons[i])) {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        Draw9Slice(boxPress, buttons[i], thickness, WHITE);
                    } else {
                        Draw9Slice(boxHover, buttons[i], thickness, WHITE);
                    }
                } else {
                    Draw9Slice(boxNeutral, buttons[i], thickness, WHITE);
                }
                Vector2 textSize = MeasureTextEx(GetFontDefault(), myString.c_str(), fontsize, fontsize / 8);
                /*DrawText(myString.c_str(), (int) (buttons[i].x + buttons[i].width / 2 - textSize.x/2),
                         (int) (buttons[i].y + buttons[i].height / 2 - textSize.y/2), fontsize, BLACK);*/
                DrawTextEx(GetFontDefault(), myString.c_str(), {(buttons[i].x + buttons[i].width / 2 - textSize.x / 2),
                                                                (buttons[i].y + buttons[i].height / 2 -
                                                                 textSize.y / 2)}, fontsize, fontsize / 8.0f, BLACK);
            }
            DrawText(TextFormat("Border thickness: %i",(int)thickness),5,5,10,BLACK);
            DrawText(TextFormat("Font size: %i", (int)fontsize),5,5+10+2,10,BLACK);
        }
        EndTextureMode();
        //The following lines put the canvas in the middle of the window and have the negative as black
        ClearBackground(BLACK);
        renderScale = std::min(GetScreenHeight() /
                               (float) canvas.texture.height, //Calculates how big or small the canvas has to be rendered.
                               GetScreenWidth() / (float) canvas.texture.width);
        renderRec.width = canvas.texture.width * renderScale;
        renderRec.height = canvas.texture.height * renderScale;
        renderRec.x = (GetScreenWidth() - renderRec.width) / 2.0f;
        renderRec.y = (GetScreenHeight() - renderRec.height) / 2.0f;
        DrawTexturePro(canvas.texture, Rectangle{0, 0, (float) canvas.texture.width, (float) -canvas.texture.height},
                       renderRec,
                       {}, 0, WHITE);
        EndDrawing();
        frameCounter++;
    } // Main game loop end

    // De-initialization here
    // ...
    // ...

    // Close window and OpenGL context
    CloseWindow();

    return EXIT_SUCCESS;
}

Vector2 calcVMouse(float scale, Rectangle canvas) {
    Vector2 r = {(GetMouseX() - canvas.x) / scale, (GetMouseY() - canvas.y) / scale};
    return r;
}

/*********************************************
* 'tex' is the texture of the 9 slice, which HAS TO be square for this to work
* 'rec' is the rectangle to use for drawing targeting. If you dont use rectangles in your code you can always construct with {x, y, width, height}
* 'thickness' describes how thick the borders of the box are supposed to be.
* 'tint' is like every other raylib function using a tint
*********************************************/
void Draw9Slice(Texture2D &tex, Rectangle rec, float thickness, Color tint) {
    //slice is a const that helps with readability of the function
    const auto slice = tex.width / 3.0f;
    //Stuffing
    DrawTexturePro(tex, {slice, slice, slice, slice},
                   {rec.x + thickness, rec.y + thickness, rec.width - thickness * 2, rec.height - thickness * 2}, {}, 0,
                   tint);
    //top left corner
    DrawTexturePro(tex, {0, 0, slice, slice}, {rec.x, rec.y, thickness, thickness}, {}, 0, tint);
    //top right corner
    DrawTexturePro(tex, {slice * 2, 0, slice, slice}, {rec.x + rec.width - thickness, rec.y, thickness, thickness}, {},
                   0, tint);
    //bottom left corner
    DrawTexturePro(tex, {0, slice * 2, slice, slice}, {rec.x, rec.y + rec.height - thickness, thickness, thickness}, {},
                   0, tint);
    //bottom right corner
    DrawTexturePro(tex, {slice * 2, slice * 2, slice, slice},
                   {rec.x + rec.width - thickness, rec.y + rec.height - thickness, thickness, thickness}, {}, 0, tint);
    //top bumper
    DrawTexturePro(tex, {slice, 0, slice, slice}, {rec.x + thickness, rec.y, rec.width - thickness * 2, thickness}, {},
                   0, tint);
    //bottom bumper
    DrawTexturePro(tex, {slice, slice * 2, slice, slice},
                   {rec.x + thickness, rec.y + rec.height - thickness, rec.width - thickness * 2, thickness}, {}, 0,
                   tint);
    //left bumper
    DrawTexturePro(tex, {0, slice, slice, slice}, {rec.x, rec.y + thickness, thickness, rec.height - thickness * 2}, {},
                   0, tint);
    //right bumper
    DrawTexturePro(tex, {slice * 2, slice, slice, slice},
                   {rec.x + rec.width - thickness, rec.y + thickness, thickness, rec.height - thickness * 2}, {}, 0,
                   tint);
}

