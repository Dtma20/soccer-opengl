#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <unistd.h>
#include "draw.h"

#define NUM_KEYS 256
#define MOVE_SPEED 0.20f

Ball ball = {{52.5f, 34.0f}, {0.0f, 0.0f}, 0.5f};
Player team1[11];
Player team2[11];
int currentPlayerTeam1 = 0;
int currentPlayerTeam2 = 0;

int lastTime = 0;
int scoreLeft = 0, scoreRight = 0;

bool keys[NUM_KEYS];

bool touch(Vector2 pos1, Vector2 pos2, float r1, float r2) {
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float distance = sqrt(dx * dx + dy * dy);
    return (distance <= r1 + r2);
}

void initPlayers() {
    team1[0] = (Player){{5.0f, 34.0f}, {0.0f, 0.0f}, 1.0f, true};
    team1[1] = (Player){{15.0f, 10.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[2] = (Player){{15.0f, 25.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[3] = (Player){{15.0f, 43.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[4] = (Player){{15.0f, 58.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[5] = (Player){{30.0f, 10.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[6] = (Player){{30.0f, 25.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[7] = (Player){{30.0f, 43.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[8] = (Player){{30.0f, 58.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[9] = (Player){{45.0f, 25.0f}, {0.0f, 0.0f}, 1.0f, false};
    team1[10] = (Player){{45.0f, 43.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[0] = (Player){{100.0f, 34.0f}, {0.0f, 0.0f}, 1.0f, true};
    team2[1] = (Player){{90.0f, 10.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[2] = (Player){{90.0f, 25.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[3] = (Player){{90.0f, 43.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[4] = (Player){{90.0f, 58.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[5] = (Player){{75.0f, 10.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[6] = (Player){{75.0f, 25.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[7] = (Player){{75.0f, 43.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[8] = (Player){{75.0f, 58.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[9] = (Player){{60.0f, 25.0f}, {0.0f, 0.0f}, 1.0f, false};
    team2[10] = (Player){{60.0f, 43.0f}, {0.0f, 0.0f}, 1.0f, false};
}

void resetGame() {
    ball.pos.x = 52.5f;
    ball.pos.y = 34.0f;
    ball.speed.x = 0.0f;
    ball.speed.y = 0.0f;
    initPlayers();
}

void handleCollision(Player *player) {

    if(!touch(ball.pos, player->pos, ball.radius, player->radius)) return;
 
    float dx = ball.pos.x - player->pos.x;
    float dy = ball.pos.y - player->pos.y;
    float distance = sqrt(dx * dx + dy * dy);
    if (distance == 0) return;
    float nx = dx / distance;
    float ny = dy / distance;

    float relSpeedX = ball.speed.x - player->speed.x;
    float relSpeedY = ball.speed.y - player->speed.y;
    float dot = relSpeedX * nx + relSpeedY * ny;
    ball.speed.x -= 2.0f * dot * nx;
    ball.speed.y -= 2.0f * dot * ny;
    ball.speed.x += player->speed.x * 0.25f;
    ball.speed.y += player->speed.y * 0.25f;

    float overlap = (ball.radius + player->radius) - distance;
    ball.pos.x += nx * overlap;
    ball.pos.y += ny * overlap;
    float speed = sqrt(ball.speed.x * ball.speed.x + ball.speed.y * ball.speed.y);
    const float maxSpeed = 0.3f;
    if (speed > maxSpeed) {
        ball.speed.x = ball.speed.x / speed * maxSpeed;
        ball.speed.y = ball.speed.y / speed * maxSpeed;
    }
}

void handlePlayerCollision(Player *p1, Player *p2) {
    float dx = p2->pos.x - p1->pos.x;
    float dy = p2->pos.y - p1->pos.y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance == 0)
        return;

    float overlap = (p1->radius + p2->radius) - distance;
    if (overlap > 0) {
        float nx = dx / distance;
        float ny = dy / distance;

        p1->pos.x -= nx * overlap / 2.0f;
        p1->pos.y -= ny * overlap / 2.0f;
        p2->pos.x += nx * overlap / 2.0f;
        p2->pos.y += ny * overlap / 2.0f;

        float dvx = p2->speed.x - p1->speed.x;
        float dvy = p2->speed.y - p1->speed.y;
        float dot = dvx * nx + dvy * ny;
        if (dot < 0) {
            float restitution = 0.8f;
            float impulse = -(1 + restitution) * dot / 2.0f;

            p1->speed.x -= impulse * nx;
            p1->speed.y -= impulse * ny;
            p2->speed.x += impulse * nx;
            p2->speed.y += impulse * ny;
        }
    }
}

void checkPlayersCollision() {

    for (int i = 0; i < 11; i++) {
        for (int j = i + 1; j < 11; j++) {
            if (touch(team1[i].pos, team1[j].pos, team1[i].radius, team1[j].radius)) {
                handlePlayerCollision(&team1[i], &team1[j]);
            }
        }
    }

    for (int i = 0; i < 11; i++) {
        for (int j = i + 1; j < 11; j++) {
            if (touch(team2[i].pos, team2[j].pos, team2[i].radius, team2[j].radius)) {
                handlePlayerCollision(&team2[i], &team2[j]);
            }
        }
    }

    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            if (touch(team1[i].pos, team2[j].pos, team1[i].radius, team2[j].radius)) {
                handlePlayerCollision(&team1[i], &team2[j]);
            }
        }
    }

    float fieldWidth = 105.0f;
    float fieldHeight = 68.0f;

    if (team1[currentPlayerTeam1].pos.x - team1[currentPlayerTeam1].radius < 0)
        team1[currentPlayerTeam1].pos.x = team1[currentPlayerTeam1].radius;
    if (team1[currentPlayerTeam1].pos.x + team1[currentPlayerTeam1].radius > fieldWidth)
        team1[currentPlayerTeam1].pos.x = fieldWidth - team1[currentPlayerTeam1].radius;

    if (team1[currentPlayerTeam1].pos.y - team1[currentPlayerTeam1].radius < 0)
        team1[currentPlayerTeam1].pos.y = team1[currentPlayerTeam1].radius;

    if (team1[currentPlayerTeam1].pos.y + team1[currentPlayerTeam1].radius > fieldHeight)
        team1[currentPlayerTeam1].pos.y = fieldHeight - team1[currentPlayerTeam1].radius;

    if (team2[currentPlayerTeam2].pos.x - team2[currentPlayerTeam2].radius < 0)
        team2[currentPlayerTeam2].pos.x = team2[currentPlayerTeam2].radius;
    if (team2[currentPlayerTeam2].pos.x + team2[currentPlayerTeam2].radius > fieldWidth)
        team2[currentPlayerTeam2].pos.x = fieldWidth - team2[currentPlayerTeam2].radius;

    if (team2[currentPlayerTeam2].pos.y - team2[currentPlayerTeam2].radius < 0)
        team2[currentPlayerTeam2].pos.y = team2[currentPlayerTeam2].radius;

    if (team2[currentPlayerTeam2].pos.y + team2[currentPlayerTeam2].radius > fieldHeight)
        team2[currentPlayerTeam2].pos.y = fieldHeight - team2[currentPlayerTeam2].radius;
    
}

int playerSwitchCooldownTeam1 = 0;
int playerSwitchCooldownTeam2 = 0;

void switchPlayerTeam1() {
    int flag = keys['e'] ? 1 : 0;
    if(!flag) return;

    if (playerSwitchCooldownTeam1 > 0)
    return;

    float minDistance = -1;
    int nearestPlayer = -1;
    for (int i = 0; i < 11; i++) {
        if (i == currentPlayerTeam1) continue;
        float dx = team1[i].pos.x - ball.pos.x;
        float dy = team1[i].pos.y - ball.pos.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (minDistance < 0 || distance < minDistance) {
            minDistance = distance;
            nearestPlayer = i;
        }
    }
    if (nearestPlayer >= 0) {
        team1[currentPlayerTeam1].controlled = false;
        currentPlayerTeam1 = nearestPlayer;
        team1[currentPlayerTeam1].controlled = true;
    }
    playerSwitchCooldownTeam1 = 200;
}

void switchPlayerTeam2() {
    int flag = keys['o'] ? 1 : 0;
    if(!flag) return;

    if (playerSwitchCooldownTeam2 > 0)
    return;

    float minDistance = -1;
    int nearestPlayer = -1;
    for (int i = 0; i < 11; i++) {
        if (i == currentPlayerTeam2) continue;
        float dx = team2[i].pos.x - ball.pos.x;
        float dy = team2[i].pos.y - ball.pos.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (minDistance < 0 || distance < minDistance) {
            minDistance = distance;
            nearestPlayer = i;
        }
    }
    if (nearestPlayer >= 0) {
        team2[currentPlayerTeam2].controlled = false;
        currentPlayerTeam2 = nearestPlayer;
        team2[currentPlayerTeam2].controlled = true;
    }
    playerSwitchCooldownTeam2 = 200;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawField();
    drawBall(ball);
    drawPlayers(team1, team2);
    drawScore(scoreLeft, scoreRight);
    glutSwapBuffers();
}


void update() {
    
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float dt = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (playerSwitchCooldownTeam1 > 0)
        playerSwitchCooldownTeam1 -= dt * 1000;
    if (playerSwitchCooldownTeam2 > 0)
        playerSwitchCooldownTeam2 -= dt * 1000;
    
    switchPlayerTeam1();
    switchPlayerTeam2();
    proccesMovement(&team1[currentPlayerTeam1], 1);
    proccesMovement(&team2[currentPlayerTeam2], 2);

    checkPlayersCollision();

    ball.pos.x += ball.speed.x * dt * 60;
    ball.pos.y += ball.speed.y * dt * 60;
    ball.speed.x *= 0.99f;
    ball.speed.y *= 0.99f;

    if (ball.pos.y - ball.radius < 0 || ball.pos.y + ball.radius > 68)
        ball.speed.y = -ball.speed.y;
    if ((ball.pos.x - ball.radius < 0 || ball.pos.x + ball.radius > 105)) {
        if (!(ball.pos.y >= 28.75f && ball.pos.y <= 39.25f))
            ball.speed.x = -ball.speed.x;
    }
    if (ball.pos.x - ball.radius <= 0 && ball.pos.y >= 28.75f && ball.pos.y <= 39.25f) {
        scoreRight++;
        resetGame();
    }
    if (ball.pos.x + ball.radius >= 105 && ball.pos.y >= 28.75f && ball.pos.y <= 39.25f) {
        scoreLeft++;
        resetGame();
    }
    for (int i = 0; i < 11; i++) {
        if (touch(ball.pos, team1[i].pos, ball.radius, team1[i].radius))
            handleCollision(&team1[i]);
        if (touch(ball.pos, team2[i].pos, ball.radius, team2[i].radius))
            handleCollision(&team2[i]);
    }

  
    team1[currentPlayerTeam1].pos.x += team1[currentPlayerTeam1].speed.x * dt * 60;
    team1[currentPlayerTeam1].pos.y += team1[currentPlayerTeam1].speed.y * dt * 60;
    team1[currentPlayerTeam1].speed.x *= 0.9f;
    team1[currentPlayerTeam1].speed.y *= 0.9f;

   
    team2[currentPlayerTeam2].pos.x += team2[currentPlayerTeam2].speed.x * dt * 60;
    team2[currentPlayerTeam2].pos.y += team2[currentPlayerTeam2].speed.y * dt * 60;
    team2[currentPlayerTeam2].speed.x *= 0.9f;
    team2[currentPlayerTeam2].speed.y *= 0.9f;

    glutPostRedisplay();
}

void timerFunc(int value) {
    update();
    glutTimerFunc(16, timerFunc, 0);
}

void keyboardDown(unsigned char key, int x, int y) {
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false; 
}

void proccesMovement(Player *player, int team) {

    if(team == 1) {
        if (keys['w']) player->speed.y = MOVE_SPEED;
        if (keys['s']) player->speed.y = -MOVE_SPEED;
        if (keys['d']) player->speed.x = MOVE_SPEED;
        if (keys['a']) player->speed.x = -MOVE_SPEED;
    }

    else {
        if (keys['i']) player->speed.y = MOVE_SPEED;
        if (keys['k']) player->speed.y = -MOVE_SPEED;
        if (keys['l']) player->speed.x = MOVE_SPEED;
        if (keys['j']) player->speed.x = -MOVE_SPEED;
    }  
}

void init() {
    glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 105.0, 0.0, 68.0);
    lastTime = glutGet(GLUT_ELAPSED_TIME);
    initPlayers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1050, 680);
    glutCreateWindow("Campo de Futebol 2D");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timerFunc, 0);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutMainLoop();
    return 0;
}