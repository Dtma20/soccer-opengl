#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <GL/glut.h>
#include "campo.h"

void drawCircle(float cx, float cy, float r, int segments, bool filled) {
    glBegin(filled ? GL_POLYGON : GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * PI * i / segments;
        float x = r * cos(theta);
        float y = r * sin(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawPenaltyArc(float cx, float cy, float r, int segments, bool leftSide) {
    float startAngle, endAngle;
    if (leftSide) {
        startAngle = -0.87f;
        endAngle = 0.87f;
    } else {
        startAngle = PI - 0.87f;
        endAngle = PI + 0.87f;
    }
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float theta = startAngle + (endAngle - startAngle) * i / segments;
        float x = r * cos(theta);
        float y = r * sin(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawGoalInsideArea(float x, float y) {
    float goalWidth = 2.5f, goalHeight = 10.5f;
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + goalWidth, y);
    glVertex2f(x + goalWidth, y + goalHeight);
    glVertex2f(x, y + goalHeight);
    glEnd();
}

void drawBall(Ball ball) {
    glColor3f(1, 1, 0);
    drawCircle(ball.pos.x, ball.pos.y, ball.radius, 30, false);
}

void drawPlayers(Player team1[], Player team2[]) {
    for (int i = 0; i < 11; i++) {
        if (team1[i].controlled) {
            glColor3f(0.0f, 1.0f, 1.0f);
        } else {
            glColor3f(0.0f, 0.0f, 1.0f);
        }
        drawCircle(team1[i].pos.x, team1[i].pos.y, team1[i].radius, 30, true);
    }

    for (int i = 0; i < 11; i++) {
        if (team2[i].controlled) {
            glColor3f(1.0f, 1.0f, 0.0f);
        } else {
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        drawCircle(team2[i].pos.x, team2[i].pos.y, team2[i].radius, 30, true);
    }
}

void drawField() {
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0, 0);
    glVertex2f(105, 0);
    glVertex2f(105, 68);
    glVertex2f(0, 68);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(52.5f, 0);
    glVertex2f(52.5f, 68);
    glEnd();
    drawCircle(52.5f, 34.0f, 9.15f, 100, false);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0, 13.85f);
    glVertex2f(16.5f, 13.85f);
    glVertex2f(16.5f, 54.15f);
    glVertex2f(0, 54.15f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(105, 13.85f);
    glVertex2f(88.5f, 13.85f);
    glVertex2f(88.5f, 54.15f);
    glVertex2f(105, 54.15f);
    glEnd();
    drawPenaltyArc(10.65f, 34.0f, 9.15f, 100, true);
    drawPenaltyArc(94.35f, 34.0f, 9.15f, 100, false);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(11.0f, 34.0f);
    glVertex2f(94.0f, 34.0f);
    glVertex2f(52.5f, 34.0f);
    glEnd();
    drawGoalInsideArea(0, 34 - (10.5f / 2.0f));
    drawGoalInsideArea(105 - 2.5f, 34 - (10.5f / 2.0f));
}

void drawScore(int scoreLeft, int scoreRight) {
    char scoreText[20];
    sprintf(scoreText, "%d - %d", scoreLeft, scoreRight);
    void* font = GLUT_BITMAP_TIMES_ROMAN_24;
    float textWidth = 0.0f;
    for (char *c = scoreText; *c != '\0'; c++) {
        textWidth += glutBitmapWidth(font, *c);
    }
    textWidth /= 10.0f;
    float posX = 52.5f - (textWidth / 2.0f);
    float posY = 60.0f;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(posX, posY);
    for (char *c = scoreText; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}