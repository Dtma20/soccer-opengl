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
    glColor3f(1, 1, 1);
    drawCircle(ball.pos.x, ball.pos.y, ball.radius, 30, true);
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
         case 0: glVertex2f(x + t/2, y + h); glVertex2f(x + w - t/2, y + h); glVertex2f(x + w - t, y + h - t); glVertex2f(x + t, y + h - t); break;
         case 1: glVertex2f(x + w, y + h - t/2); glVertex2f(x + w, y + h/2 + t/2); glVertex2f(x + w - t, y + h/2 + t); glVertex2f(x + w - t, y + h - t); break;
         case 2: glVertex2f(x + w, y + h/2 - t/2); glVertex2f(x + w, y + t/2); glVertex2f(x + w - t, y + t); glVertex2f(x + w - t, y + h/2 - t); break;
         case 3: glVertex2f(x + t/2, y); glVertex2f(x + w - t/2, y); glVertex2f(x + w - t, y + t); glVertex2f(x + t, y + t); break;
         case 4: glVertex2f(x, y + h/2 - t/2); glVertex2f(x, y + t/2); glVertex2f(x + t, y + t); glVertex2f(x + t, y + h/2 - t); break;
         case 5: glVertex2f(x, y + h - t/2); glVertex2f(x, y + h/2 + t/2); glVertex2f(x + t, y + h/2 + t); glVertex2f(x + t, y + h - t); break;
         case 6: glVertex2f(x + t, y + h/2 + t/2); glVertex2f(x + w - t, y + h/2 + t/2); glVertex2f(x + w - t/2, y + h/2); glVertex2f(x + w - t, y + h/2 - t/2); glVertex2f(x + t, y + h/2 - t/2); glVertex2f(x + t/2, y + h/2); break;
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
    float size = 1.8f;
    float digitWidth = size;
    float digitHeight = size * 2;
    float gap = size * 1.0f;

    float totalWidth = digitWidth + gap + gap + digitWidth;
    float startX = (105.0f - totalWidth) / 2.0f;
    float startY = 68.0f - digitHeight - 2.5f;

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