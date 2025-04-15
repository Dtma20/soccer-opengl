#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>
#include "campo.h"
#include "texture.h"

void drawField();
void drawBall(Ball ball, float ballAngle);
void drawGoalInsideArea(float x, float y);
void drawScore(int scoreLeft, int scoreRight);
void drawPlayers(Player team1[], Player team2[]);
void drawPlayer(Player p, int type, GLuint Texture);
void drawCircle(float cx, float cy, float r, int segments, bool filled);
void drawPenaltyArc(float cx, float cy, float r, int segments, bool leftSide);

#endif