#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <raylib.h>

const uint8_t map[10][10] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

#define TILE_SIZE           40
#define DEFAULT_RAY_LENGTH 250

bool IsHit(const uint8_t map[10][10], Vector2 point, float size) {
    for (uint8_t row = 0; row < 10; row++) {
        for (uint8_t col = 0; col < 10; col++) {
            if(col < point.x + size &&
               col + size > point.x &&
               row < point.y + size &&
               row + size > point.y && map[row][col] == 1) {
                return true;
            }   
        }
    }

    return false;
}

void StepRay(const Vector2 position, Vector2 forward, const int stepCount, const int stepLength, int *incr, Color color, Vector2 *pHit) {
    Vector2 start = (Vector2) {
        position.x,
        position.y
    };

    Vector2 end = (Vector2) {
        (position.x + (forward.x / stepLength)),
        (position.y + (forward.y / stepLength))
    };
    
    pHit->x = end.x;
    pHit->y = end.y;
    
    DrawLine(start.x * TILE_SIZE + TILE_SIZE/2, start.y * TILE_SIZE + TILE_SIZE/2, end.x * TILE_SIZE + TILE_SIZE/2, end.y * TILE_SIZE + TILE_SIZE/2, GRAY);

    if (!IsHit(map, end, 0.5) && *incr < stepCount) {
        *incr += 1;
        StepRay(end, forward, stepCount, stepLength, incr, color, pHit);
    } else {
        *incr = 0;
    }
}

float Dist(const Vector2 v1, const Vector2 v2) {
    return powf(v1.x - v2.x, 2) + powf(v1.y - v2.y, 2);
}

void Render3DMap(Vector2 cameraPosition, float cameraRotation, int lineThickness, int fov) {
    for (int i = -fov/2; i < fov/2; i++) {
        int c = 0;
        Vector2 hit;
        
        Vector2 dir = (Vector2) { sin((cameraRotation + i) * (PI/180)), cos((cameraRotation + i) * (PI/180)) };

        StepRay(cameraPosition, dir, 1000, 100, &c, GRAY, &hit);

        float dist = Dist(cameraPosition, hit);

        Color color = { 150 - dist * 1.5, 150 - dist * 1.5, 150 - dist * 1.5, 255 };

        DrawRectangle((i * lineThickness + (lineThickness * fov/2) + (10*TILE_SIZE)), 5*TILE_SIZE - (1000 / dist)/2, lineThickness, (1000 / dist), color);
    }
}

void Render2DMap(const uint8_t map[10][10]) {
    for (uint8_t row = 0; row < 10; row++) {
        for (uint8_t col = 0; col < 10; col++) {
            Color color = BLACK;
            if (map[row][col] == 1) {
                color = BROWN;
            }

            DrawRectangle(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE, color);
        }
    }
}

void Render2DPlayer(const Vector2 position) {
    DrawCircle(position.x * TILE_SIZE + TILE_SIZE/2, position.y * TILE_SIZE + TILE_SIZE/2, 6, ORANGE);
}

Vector2 Update2DPlayer(Vector2 *position, int *rotation) {
    Vector2 forward = (Vector2) { sin(*rotation * (PI/180)), cos(*rotation * (PI/180)) };

    Vector2 velocity = (Vector2) { 0, 0 };

    if (IsKeyDown(KEY_W)) {
        velocity = (Vector2) {
            0.01f * forward.x,
            0.01f * forward.y
        };
    }

    if (IsKeyDown(KEY_S)) {
        velocity = (Vector2) {
            -0.01f * forward.x, 
            -0.01f * forward.y
        };
    }

    if (IsKeyDown(KEY_Q)) {
        (*rotation)--;
    }

    if (IsKeyDown(KEY_E)) {
        (*rotation)++;
    }

    if (!IsHit(map, (Vector2) { position->x + velocity.x, position->y + velocity.y }, 0.5)) {
        position->x += velocity.x;
        position->y += velocity.y;
    } else {
        
    }
    
    return forward;
}

int main(void) {
    int lineWidth = 10;
    int windowWidth = (10 * TILE_SIZE) + (lineWidth * 60);
    InitWindow(windowWidth, 10*TILE_SIZE, "raycasting");
    
    Vector2 playerPosition = (Vector2) { 1, 1 };
    Vector2 playerForward = {};
    int playerRotation = 0;
    
    double lastTime = GetTime();
    while (!WindowShouldClose()) {
        double currentTime = GetTime();

        if (currentTime - lastTime > 1.0f/150.0f) {
            lastTime = currentTime;
            playerForward = Update2DPlayer(&playerPosition, &playerRotation);
        }
        
        BeginDrawing();

        ClearBackground(BLACK);

        Render2DMap(map);
        Render2DPlayer(playerPosition);
        Render3DMap(playerPosition, playerRotation, lineWidth, 60);

        EndDrawing();

    }

    CloseWindow();
    return 0;
}
