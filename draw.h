#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>
#include "campo.h"

void drawBall(Ball ball);
void drawField();
void drawGoalInsideArea(float x, float y);
void drawScore(int scoreLeft, int scoreRight);
void drawPlayers(Player team1[], Player team2[]);
void drawCircle(float cx, float cy, float r, int segments, bool filled);
void drawPenaltyArc(float cx, float cy, float r, int segments, bool leftSide);

#endif