#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <GL/glut.h>
#include "campo.h"
#include "bresenham.h"

#define X_OFFSET 75
#define Y_OFFSET 60

void drawCircle(float cx, float cy, float r, int segments, bool filled) {
    cx += X_OFFSET;
    cy += Y_OFFSET;
    if (!filled) {
        glPointSize(3.0f);
        glBegin(GL_POINTS);
            bresenham_arc((int)cx, (int)cy, (int)r, 0, 360);
        glEnd();
    } else {
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int i = 0; i <= segments; i++) {
                float theta = 2.0f * PI * i / segments;
                float x = r * cos(theta);
                float y = r * sin(theta);
                glVertex2f(cx + x, cy + y);
            }
        glEnd();
    }
}

void drawPenaltyArc(float cx, float cy, float r, int segments, bool leftSide) {
    cx += X_OFFSET;
    cy += Y_OFFSET;
    float startRad, endRad;
    if (leftSide) {
        startRad = 2 * PI - 0.87f;
        endRad   = 0.87f;
    } else {
        startRad = PI - 0.87f;
        endRad   = PI + 0.87f;
    }
    int startDeg = (int)(startRad * 180.0f / PI);
    int endDeg   = (int)(endRad   * 180.0f / PI);
    
    glPointSize(3.0f);
    glBegin(GL_POINTS);
        bresenham_arc((int)cx, (int)cy, (int)r, startDeg, endDeg);
    glEnd();
}

void drawGoalInsideArea(float x, float y) {
    x += X_OFFSET;
    y += Y_OFFSET;
    float goalWidth = 25.0f, goalHeight = 105.0f;
    glPointSize(3.0f);
    glBegin(GL_POINTS);
        bresenham_line((int)x, (int)y, (int)(x + goalWidth), (int)y);
        bresenham_line((int)(x + goalWidth), (int)y, (int)(x + goalWidth), (int)(y + goalHeight));
        bresenham_line((int)(x + goalWidth), (int)(y + goalHeight), (int)x, (int)(y + goalHeight));
        bresenham_line((int)x, (int)(y + goalHeight), (int)x, (int)y);
    glEnd();
}

void drawBall(Ball ball) {
    glColor3f(1, 1, 1);
    drawCircle(ball.pos.x, ball.pos.y, ball.radius, 30, true);
}

void drawPlayers(Player team1[], Player team2[]) {
    for (int i = 0; i < 11; i++) {
        glColor3f(team1[i].controlled ? 0.0f : 0.0f, team1[i].controlled ? 1.0f : 0.0f, 1.0f);
        drawCircle(team1[i].pos.x, team1[i].pos.y, team1[i].radius, 30, true);
    }
    for (int i = 0; i < 11; i++) {
        glColor3f(1.0f, team2[i].controlled ? 1.0f : 0.0f, 0.0f);
        drawCircle(team2[i].pos.x, team2[i].pos.y, team2[i].radius, 30, true);
    }
}

void drawCornerArcs() {
    int radius = 30;
    glPointSize(3.0f);
    glColor3f(1, 1, 1);
    glBegin(GL_POINTS);
        bresenham_arc(0 + X_OFFSET, 0 + Y_OFFSET, radius, 0, 90);
        bresenham_arc(1050 + X_OFFSET, 0 + Y_OFFSET, radius, 90, 180);
        bresenham_arc(1050 + X_OFFSET, 680 + Y_OFFSET, radius, 180, 270);
        bresenham_arc(0 + X_OFFSET, 680 + Y_OFFSET, radius, 270, 360);
    glEnd();
}

void drawField() {
    glColor3f(1, 1, 1);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
        bresenham_line(0 + X_OFFSET, 0 + Y_OFFSET, 1050 + X_OFFSET, 0 + Y_OFFSET);
        bresenham_line(1050 + X_OFFSET, 0 + Y_OFFSET, 1050 + X_OFFSET, 680 + Y_OFFSET);
        bresenham_line(1050 + X_OFFSET, 680 + Y_OFFSET, 0 + X_OFFSET, 680 + Y_OFFSET);
        bresenham_line(0 + X_OFFSET, 680 + Y_OFFSET, 0 + X_OFFSET, 0 + Y_OFFSET);
        bresenham_line(525 + X_OFFSET, 0 + Y_OFFSET, 525 + X_OFFSET, 680 + Y_OFFSET);
    glEnd();

    drawCircle(525.0f, 340.0f, 91.5f, 100, false);

    glPointSize(3.0f);
    glBegin(GL_POINTS);
        bresenham_line(0 + X_OFFSET, 138 + Y_OFFSET, 165 + X_OFFSET, 138 + Y_OFFSET);
        bresenham_line(165 + X_OFFSET, 138 + Y_OFFSET, 165 + X_OFFSET, 541 + Y_OFFSET);
        bresenham_line(165 + X_OFFSET, 541 + Y_OFFSET, 0 + X_OFFSET, 541 + Y_OFFSET);
        bresenham_line(0 + X_OFFSET, 541 + Y_OFFSET, 0 + X_OFFSET, 138 + Y_OFFSET);

        bresenham_line(1050 + X_OFFSET, 138 + Y_OFFSET, 885 + X_OFFSET, 138 + Y_OFFSET);
        bresenham_line(885 + X_OFFSET, 138 + Y_OFFSET, 885 + X_OFFSET, 541 + Y_OFFSET);
        bresenham_line(885 + X_OFFSET, 541 + Y_OFFSET, 1050 + X_OFFSET, 541 + Y_OFFSET);
        bresenham_line(1050 + X_OFFSET, 541 + Y_OFFSET, 1050 + X_OFFSET, 138 + Y_OFFSET);
    glEnd();

    drawPenaltyArc(106.5f, 340.0f, 91.5f, 100, true);
    drawPenaltyArc(943.5f, 340.0f, 91.5f, 100, false);

    glPointSize(3.0f);
    glBegin(GL_POINTS);
        glVertex2f(110.0f + X_OFFSET, 340.0f + Y_OFFSET);
        glVertex2f(940.0f + X_OFFSET, 340.0f + Y_OFFSET);
        glVertex2f(525.0f + X_OFFSET, 340.0f + Y_OFFSET);
    glEnd();

    drawGoalInsideArea(0, 287.5f);
    drawGoalInsideArea(1025, 287.5f);
    drawCornerArcs();
}

int digits[10][7] = {
    {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1},
    {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
};

void drawSegmentPolygon(int seg, float x, float y, float size) {
    float w = size;
    float h = size * 2;
    float t = size * 0.18f;

    glBegin(GL_POLYGON);
    switch (seg) {
         case 0: glVertex2f(x + t/2, y + h); glVertex2f(x + w - t/2, y + h);
                 glVertex2f(x + w - t, y + h - t); glVertex2f(x + t, y + h - t); break;
         case 1: glVertex2f(x + w, y + h - t/2); glVertex2f(x + w, y + h/2 + t/2);
                 glVertex2f(x + w - t, y + h/2 + t); glVertex2f(x + w - t, y + h - t); break;
         case 2: glVertex2f(x + w, y + h/2 - t/2); glVertex2f(x + w, y + t/2);
                 glVertex2f(x + w - t, y + t); glVertex2f(x + w - t, y + h/2 - t); break;
         case 3: glVertex2f(x + t/2, y); glVertex2f(x + w - t/2, y);
                 glVertex2f(x + w - t, y + t); glVertex2f(x + t, y + t); break;
         case 4: glVertex2f(x, y + h/2 - t/2); glVertex2f(x, y + t/2);
                 glVertex2f(x + t, y + t); glVertex2f(x + t, y + h/2 - t); break;
         case 5: glVertex2f(x, y + h - t/2); glVertex2f(x, y + h/2 + t/2);
                 glVertex2f(x + t, y + h/2 + t); glVertex2f(x + t, y + h - t); break;
         case 6: glVertex2f(x + t, y + h/2 + t/2); glVertex2f(x + w - t, y + h/2 + t/2);
                 glVertex2f(x + w - t/2, y + h/2); glVertex2f(x + w - t, y + h/2 - t/2);
                 glVertex2f(x + t, y + h/2 - t/2); glVertex2f(x + t/2, y + h/2); break;
    }
    glEnd();
}

void drawDigit(int digit, float x, float y, float size) {
    int valid_digit = digit % 10;
    if (valid_digit < 0) valid_digit = 0;

    if (valid_digit >= 0 && valid_digit <= 9) {
        for (int i = 0; i < 7; i++) {
            if (digits[valid_digit][i]) {
                drawSegmentPolygon(i, x, y, size);
            }
        }
    }
}

void drawScore(int left, int right) {
    float size = 18.0f;
    float digitWidth = size;
    float digitHeight = size * 2;
    float gap = size * 1.0f;

    int offsetX = 75;
    int offsetY = 60;

    float totalWidth = digitWidth + gap + gap + digitWidth;
    float startX = offsetX + (1050.0f - totalWidth) / 2.0f;
    float startY = offsetY + 680.0f - digitHeight - 25.0f;

    int leftUnits = abs(left % 10);
    int rightUnits = abs(right % 10);

    glColor3f(1.0f, 1.0f, 1.0f);

    drawDigit(leftUnits, startX, startY, size);

    float colonX = startX + digitWidth + gap;
    float colonDotSize = size * 0.2f;
    glRectf(colonX - colonDotSize / 2, startY + digitHeight * 0.66f - colonDotSize / 2,
            colonX + colonDotSize / 2, startY + digitHeight * 0.66f + colonDotSize / 2);
    glRectf(colonX - colonDotSize / 2, startY + digitHeight * 0.33f - colonDotSize / 2,
            colonX + colonDotSize / 2, startY + digitHeight * 0.33f + colonDotSize / 2);

    drawDigit(rightUnits, startX + digitWidth + 2 * gap, startY, size);
}
