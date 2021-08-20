#include "raylib.h"

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

const int windowWidth = 512;
const int windowHeight = 380;

Texture2D nebula;
Texture2D scarfy;
Texture2D background;
Texture2D midground;
Texture2D foreground;

const int gravity = 1250;

const int nebulaeSize = 3;
AnimData nebulae[nebulaeSize];
AnimData scarfyData;

float bgX;
float mgX;
float fgX;

const int nebVel = -200;
int velocity;
const int jumpVel = -700;

float score;
bool isGameOver;

bool isGrounded() {
    return scarfyData.pos.y >= windowHeight - scarfyData.rec.height;
}

void updateAnimData(AnimData& data, float dt, int maxFrame) {
    data.runningTime += dt;
    if (data.runningTime >= data.updateTime) {
        data.runningTime = 0.0f;
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        data.frame %= maxFrame;
    }
}

void init() {
    for (int i = 0; i < nebulaeSize; i++) {
        nebulae[i] = {
            {0.0f, 0.0f, nebula.width / 8.0f, nebula.height / 8.0f},
            {windowWidth + 300.0f * i, windowHeight - nebula.height / 8.0f},
            0,
            0,
            0,
        };
    }

    scarfyData = {
        { 0, 0, (float)(scarfy.width / 6), (float)scarfy.height },
        { windowWidth / 2 - (float)(scarfy.width / 6) / 2, windowHeight - (float)scarfy.height },
        0,
        1.0f / 12.0f,
        0.0f,
    };

    score = 0.0f;
}

int main() {
    InitWindow(windowWidth, windowHeight, "Dapper Dasher");

    nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    scarfy = LoadTexture("textures/scarfy.png");
    background = LoadTexture("textures/far-buildings.png");
    midground = LoadTexture("textures/back-buildings.png");
    foreground = LoadTexture("textures/foreground.png");

    init();

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (!isGameOver) {
            const float dt = GetFrameTime();

            // Increase score
            score += dt;

            // Apply gravity (acceleration)
            if (isGrounded()) velocity = 0;
            else velocity += gravity * dt;

            // Jump
            if (IsKeyPressed(KEY_SPACE) && isGrounded()) velocity += jumpVel;

            // Update nebula position
            for (int i = 0; i < nebulaeSize; i++) {
                nebulae[i].pos.x += nebVel * dt;
                if (nebulae[i].pos.x <= -nebulae[i].rec.width) nebulae[i].pos.x = windowWidth;
            }

            // Update scarfy position
            scarfyData.pos.y += velocity * dt;

            // Update scarfy animation frame
            if (isGrounded()) {
                updateAnimData(scarfyData, dt, 5);
            }

            // Update nebula animation frame
            for (int i = 0; i < nebulaeSize; i++) {
                updateAnimData(nebulae[i], dt, 7);
            }

            // Collision Detection
            for (AnimData nebula : nebulae) {
                float nebPad = 20.0f;
                Rectangle nebRec = {
                    nebula.pos.x + nebPad,
                    nebula.pos.y + nebPad,
                    nebula.rec.width - 2 * nebPad,
                    nebula.rec.height - 2 * nebPad
                };
                float scarfyPad = 30.0f;
                Rectangle scarfyRec = {
                    scarfyData.pos.x + scarfyPad,
                    scarfyData.pos.y + scarfyPad,
                    scarfyData.rec.width - 2 * scarfyPad,
                    scarfyData.rec.height - 2 * scarfyPad
                };
                if (CheckCollisionRecs(nebRec, scarfyRec)) {
                    isGameOver = true;
                }
            }

            // Scroll BG
            bgX -= 20 * dt;
            if (bgX <= -background.width * 2) bgX = 0.0f;
            // Scroll MG
            mgX -= 40 * dt;
            if (mgX <= -midground.width * 2) mgX = 0.0f;
            // Scroll FG
            fgX -= 80 * dt;
            if (fgX <= -foreground.width * 2) fgX = 0.0f;
        }

        BeginDrawing();
        ClearBackground(WHITE);

        DrawTextureEx(background, (Vector2) { bgX, 0.0f }, 0.0f, 2.0f, WHITE);
        DrawTextureEx(background, (Vector2) { bgX + background.width * 2, 0.0f }, 0.0f, 2.0f, WHITE);
        DrawTextureEx(midground, (Vector2) { mgX, 0.0f }, 0.0f, 2.0f, WHITE);
        DrawTextureEx(midground, (Vector2) { mgX + midground.width * 2, 0.0f }, 0.0f, 2.0f, WHITE);
        DrawTextureEx(foreground, (Vector2) { fgX, 0.0f }, 0.0f, 2.0f, WHITE);
        DrawTextureEx(foreground, (Vector2) { fgX + foreground.width * 2, 0.0f }, 0.0f, 2.0f, WHITE);

        for (int i = 0; i < nebulaeSize; i++) {
            DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
        }

        DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);

        DrawText(TextFormat("Score: %i", (int)score), 10, 10, 20, SKYBLUE);

        if (isGameOver) {
            DrawText(
                "Game Over!",
                (windowWidth - MeasureText("Game Over!", 50)) / 2,
                windowHeight / 2 - 50,
                50,
                VIOLET
            );
            DrawText(
                "Press [SPACE] to retry",
                (windowWidth - MeasureText("Press [SPACE] to retry", 20)) / 2,
                windowHeight / 2,
                20,
                VIOLET
            );

            if (IsKeyPressed(KEY_SPACE)) {
                isGameOver = false;
                init();
            }
        }

        EndDrawing();
    }

    UnloadTexture(nebula);
    UnloadTexture(scarfy);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();

    return 0;
}
