#ifndef CAMPO_H
#define CAMPO_H
#include "texture.h"

#define PI 3.14159265358979323846f

typedef struct {
    float x, y;
} Vector2;

typedef struct {
    Vector2 pos;
    Vector2 speed;
    float radius;
} Ball;

typedef struct {
    Vector2 pos;
    Vector2 speed;
    float radius;
    bool controlled;
} Player;

void proccesMovement(Player *player, int team);

#endif 