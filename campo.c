#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <unistd.h>
#include "draw.h"

#define NUM_KEYS 256
#define FIELD_WIDTH 1050.0f
#define FIELD_HEIGHT 680.0f
#define GOAL_BOTTOM_Y 287.5f
#define GOAL_TOP_Y 392.5f
#define MOVE_SPEED 110.0f
#define SHOOT_FORCE 300.0f
#define SPEED_THRESHOLD 0.1f
#define PLAYER_SWITCH_COOLDOWN_MS 150
#define POSSESSION_COOLDOWN_MS 1000
#define STEAL_THRESHOLD 40.0f
#define BALL_POSSESSION_OFFSET 2.0f
#define FRICTION_BALL 0.98f
#define PASS_FORCE 180.0f


Player team1[11];
Player team2[11];
float ballAngle = 0.0f;
int possessionCooldown = 0;
int currentPlayerTeam1 = 9;
int currentPlayerTeam2 = 10;
Ball ball = {{FIELD_WIDTH / 2.0f, FIELD_HEIGHT / 2.0f}, {0.0f, 0.0f}, 7.0f};

int lastTime = 0;
int scoreLeft = 0, scoreRight = 0;
bool keys[NUM_KEYS];

Player *ballHolder = NULL;

bool touch(Vector2 pos1, Vector2 pos2, float r1, float r2) {
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float distanceSquared = dx * dx + dy * dy;
    float radiiSum = r1 + r2;
    return (distanceSquared <= radiiSum * radiiSum);
}

void tryPass(Player *players, Player *player, int team) {
    bool passKeyPressed = (team == 1 && keys['r']) || (team == 2 && keys['p']);

    if (passKeyPressed && ballHolder == player) {
        float minDist = 1e9;
        Player *target = NULL;

        for (int i = 0; i < 11; i++) {
            Player *teammate = &players[i];
            if (teammate == player) continue;

            float dx = teammate->pos.x - player->pos.x;
            float dy = teammate->pos.y - player->pos.y;
            float dist = dx * dx + dy * dy;

            if (dist < minDist) {
                minDist = dist;
                target = teammate;
            }
        }

        if (target) {
            float dx = target->pos.x - player->pos.x;
            float dy = target->pos.y - player->pos.y;
            float len = sqrt(dx * dx + dy * dy);

            if (len > 0.01f) {
                dx /= len;
                dy /= len;
            }

            ball.speed.x = dx * PASS_FORCE;
            ball.speed.y = dy * PASS_FORCE;

            ballHolder = NULL;
            possessionCooldown = POSSESSION_COOLDOWN_MS;
        }
    }
}


void initPlayers() {
    team1[0] = (Player){{25.0f, FIELD_HEIGHT / 2.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[1] = (Player){{190.0f, 100.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[2] = (Player){{190.0f, 215.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[3] = (Player){{190.0f, 460.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[4] = (Player){{190.0f, 580.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[5] = (Player){{330.0f, 180.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[6] = (Player){{330.0f, FIELD_HEIGHT / 2.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[7] = (Player){{330.0f, 500.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[8] = (Player){{480.0f, 180.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[9] = (Player){{480.0f, FIELD_HEIGHT / 2.0f}, {0.0f, 0.0f}, 15.0f, false};
    team1[10] = (Player){{480.0f, 500.0f}, {0.0f, 0.0f}, 15.0f, false};

    team2[0] = (Player){{FIELD_WIDTH - 25.0f, FIELD_HEIGHT / 2.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[1] = (Player){{FIELD_WIDTH - 190.0f, 100.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[2] = (Player){{FIELD_WIDTH - 190.0f, 215.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[3] = (Player){{FIELD_WIDTH - 190.0f, 460.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[4] = (Player){{FIELD_WIDTH - 190.0f, 580.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[5] = (Player){{FIELD_WIDTH - 330.0f, 180.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[6] = (Player){{FIELD_WIDTH - 330.0f, FIELD_HEIGHT / 2.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[7] = (Player){{FIELD_WIDTH - 330.0f, 500.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[8] = (Player){{FIELD_WIDTH - 480.0f, 180.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[9] = (Player){{FIELD_WIDTH - 480.0f, FIELD_HEIGHT / 2.0f}, {0.0f, 0.0f}, 15.0f, false};
    team2[10] = (Player){{FIELD_WIDTH - 480.0f, 500.0f}, {0.0f, 0.0f}, 15.0f, false};

    team1[currentPlayerTeam1].controlled = true;
    for(int i=0; i<11; ++i) if(i != currentPlayerTeam1) team1[i].controlled = false;
    team2[currentPlayerTeam2].controlled = true;
    for(int i=0; i<11; ++i) if(i != currentPlayerTeam2) team2[i].controlled = false;
}


void resetGame() {
    ball.pos.x = FIELD_WIDTH / 2.0f;
    ball.pos.y = FIELD_HEIGHT / 2.0f;
    ball.speed.x = 0.0f;
    ball.speed.y = 0.0f;
    ballHolder = NULL;
    possessionCooldown = 0;
    currentPlayerTeam1 = 9;
    currentPlayerTeam2 = 10;
    initPlayers();
}

bool isPlayerInTeam1(Player *p) {
    for (int i = 0; i < 11; i++) {
        if (p == &team1[i])
            return true;
    }
    return false;
}

bool isPlayerInTeam2(Player *p) {
    for (int i = 0; i < 11; i++) {
        if (p == &team2[i])
            return true;
    }
    return false;
}

void tryShoot(Player *player, int team) {
    bool shootKeyPressed = (team == 1 && keys['q']) || (team == 2 && keys['u']);

    if (shootKeyPressed && ballHolder == player) {
        float goalTargetX = (team == 1) ? FIELD_WIDTH : 0.0f;
        float goalTargetY = (GOAL_BOTTOM_Y + GOAL_TOP_Y) / 2.0f;

        float shootDirectionX = goalTargetX - player->pos.x;
        float shootDirectionY = goalTargetY - player->pos.y;

        float dirLen = sqrt(shootDirectionX * shootDirectionX + shootDirectionY * shootDirectionY);
        if (dirLen > 0.01f) {
            shootDirectionX /= dirLen;
            shootDirectionY /= dirLen;
        } else {
            shootDirectionX = (team == 1) ? 1.0f : -1.0f;
            shootDirectionY = 0.0f;
        }

        ball.speed.x = shootDirectionX * SHOOT_FORCE;
        ball.speed.y = shootDirectionY * SHOOT_FORCE;
        
        ballHolder = NULL;
        possessionCooldown = POSSESSION_COOLDOWN_MS;
    }
}



void handlePlayerCollision(Player *p1, Player *p2) {
    float dx = p2->pos.x - p1->pos.x;
    float dy = p2->pos.y - p1->pos.y;
    float distance = sqrt(dx * dx + dy * dy);
    float radiiSum = p1->radius + p2->radius;

    if (distance < radiiSum && distance > 0.001f) {
        float overlap = radiiSum - distance;
        float nx = dx / distance;
        float ny = dy / distance;

        p1->pos.x -= nx * overlap * 0.5f;
        p1->pos.y -= ny * overlap * 0.5f;
        p2->pos.x += nx * overlap * 0.5f;
        p2->pos.y += ny * overlap * 0.5f;

        float dvx = p1->speed.x - p2->speed.x;
        float dvy = p1->speed.y - p2->speed.y;
        float relativeVelocityNormal = dvx * nx + dvy * ny;

        if (relativeVelocityNormal > 0) {
             float restitution = 0.5f;
             float impulseMagnitude = -(1.0f + restitution) * relativeVelocityNormal / 2.0f;

             p1->speed.x += impulseMagnitude * nx;
             p1->speed.y += impulseMagnitude * ny;
             p2->speed.x -= impulseMagnitude * nx;
             p2->speed.y -= impulseMagnitude * ny;
        }
    }
}

void checkPlayersCollision() {
    for (int i = 0; i < 11; i++) {
        for (int j = i + 1; j < 11; j++) {
            handlePlayerCollision(&team1[i], &team1[j]);
        }
        for (int j = i + 1; j < 11; j++) {
             handlePlayerCollision(&team2[i], &team2[j]);
        }
        for (int j = 0; j < 11; j++) {
            handlePlayerCollision(&team1[i], &team2[j]);
        }
    }

    for (int i = 0; i < 11; i++) {
        if (team1[i].pos.x - team1[i].radius < 0) team1[i].pos.x = team1[i].radius;
        if (team1[i].pos.x + team1[i].radius > FIELD_WIDTH) team1[i].pos.x = FIELD_WIDTH - team1[i].radius;
        if (team1[i].pos.y - team1[i].radius < 0) team1[i].pos.y = team1[i].radius;
        if (team1[i].pos.y + team1[i].radius > FIELD_HEIGHT) team1[i].pos.y = FIELD_HEIGHT - team1[i].radius;

        if (team2[i].pos.x - team2[i].radius < 0) team2[i].pos.x = team2[i].radius;
        if (team2[i].pos.x + team2[i].radius > FIELD_WIDTH) team2[i].pos.x = FIELD_WIDTH - team2[i].radius;
        if (team2[i].pos.y - team2[i].radius < 0) team2[i].pos.y = team2[i].radius;
        if (team2[i].pos.y + team2[i].radius > FIELD_HEIGHT) team2[i].pos.y = FIELD_HEIGHT - team2[i].radius;
    }
}


int playerSwitchCooldownTeam1 = 0;
int playerSwitchCooldownTeam2 = 0;

void switchPlayerTeam1() {
    if (!keys['e'] || playerSwitchCooldownTeam1 > 0)
        return;

    float minDistanceSq = -1.0f;
    int nearestPlayer = -1;

    for (int i = 0; i < 11; i++) {
        if (i == currentPlayerTeam1) continue;

        float dx = team1[i].pos.x - ball.pos.x;
        float dy = team1[i].pos.y - ball.pos.y;
        float distanceSq = dx * dx + dy * dy;

        if (nearestPlayer == -1 || distanceSq < minDistanceSq) {
            minDistanceSq = distanceSq;
            nearestPlayer = i;
        }
    }

    if (nearestPlayer != -1) {
        team1[currentPlayerTeam1].controlled = false;
        team1[currentPlayerTeam1].speed = (Vector2){0.0f, 0.0f};
        currentPlayerTeam1 = nearestPlayer;
        team1[currentPlayerTeam1].controlled = true;
        playerSwitchCooldownTeam1 = PLAYER_SWITCH_COOLDOWN_MS;
    }
}

void switchPlayerTeam2() {
     if (!keys['o'] || playerSwitchCooldownTeam2 > 0)
        return;

    float minDistanceSq = -1.0f;
    int nearestPlayer = -1;

    for (int i = 0; i < 11; i++) {
        if (i == currentPlayerTeam2) continue;

        float dx = team2[i].pos.x - ball.pos.x;
        float dy = team2[i].pos.y - ball.pos.y;
        float distanceSq = dx * dx + dy * dy;

        if (nearestPlayer == -1 || distanceSq < minDistanceSq) {
            minDistanceSq = distanceSq;
            nearestPlayer = i;
        }
    }

    if (nearestPlayer != -1) {
        team2[currentPlayerTeam2].controlled = false;
        team2[currentPlayerTeam2].speed = (Vector2){0.0f, 0.0f};
        currentPlayerTeam2 = nearestPlayer;
        team2[currentPlayerTeam2].controlled = true;
        playerSwitchCooldownTeam2 = PLAYER_SWITCH_COOLDOWN_MS;
    }
}

void updateBall(float dt) {
    if (ballHolder != NULL) {
        float holderSpeedMag = sqrt(ballHolder->speed.x * ballHolder->speed.x + ballHolder->speed.y * ballHolder->speed.y);
        float offsetX = 0.0f, offsetY = 0.0f;
        float totalRadius = ballHolder->radius + ball.radius + BALL_POSSESSION_OFFSET;

        if (holderSpeedMag > SPEED_THRESHOLD) {
            offsetX = (ballHolder->speed.x / holderSpeedMag) * totalRadius;
            offsetY = (ballHolder->speed.y / holderSpeedMag) * totalRadius;
        } else {
             float forwardDir = isPlayerInTeam1(ballHolder) ? 1.0f : -1.0f;
             offsetX = forwardDir * totalRadius;
             offsetY = -10.0f;
        }

        ball.pos.x = ballHolder->pos.x + offsetX;
        ball.pos.y = ballHolder->pos.y + offsetY;
        ball.speed = ballHolder->speed;
    } else {
        ball.pos.x += ball.speed.x * dt;
        ball.pos.y += ball.speed.y * dt;

        ball.speed.x *= pow(FRICTION_BALL, dt * 60.0f);
        ball.speed.y *= pow(FRICTION_BALL, dt * 60.0f);


        if (ball.pos.y - ball.radius < 0) {
            ball.pos.y = ball.radius;
            ball.speed.y = -ball.speed.y * 0.8f;
        } else if (ball.pos.y + ball.radius > FIELD_HEIGHT) {
            ball.pos.y = FIELD_HEIGHT - ball.radius;
            ball.speed.y = -ball.speed.y * 0.8f;
        }

        bool isBehindGoalLineLeft = ball.pos.x - ball.radius < 0;
        bool isBehindGoalLineRight = ball.pos.x + ball.radius > FIELD_WIDTH;
        bool isInGoalHeight = (ball.pos.y >= GOAL_BOTTOM_Y && ball.pos.y <= GOAL_TOP_Y);

        if (isBehindGoalLineLeft) {
            if (isInGoalHeight) {
                scoreRight++;
                resetGame();
                return;
            } else {
                ball.pos.x = ball.radius;
                ball.speed.x = -ball.speed.x * 0.8f;
            }
        } else if (isBehindGoalLineRight) {
            if (isInGoalHeight) {
                scoreLeft++;
                resetGame();
                return;
            } else {
                ball.pos.x = FIELD_WIDTH - ball.radius;
                ball.speed.x = -ball.speed.x * 0.8f;
            }
        }
    }
}

void checkBallPossession() {
    if (possessionCooldown > 0) {
        return;
    }

    float possessionThreshold = ball.radius + 10.0f + 5.0f;
    float stealDistanceSq = STEAL_THRESHOLD * STEAL_THRESHOLD;

    if (ballHolder != NULL) {
        Player *opponentTeam = isPlayerInTeam1(ballHolder) ? team2 : team1;
        int *opponentCurrentPlayerIdx = isPlayerInTeam1(ballHolder) ? &currentPlayerTeam2 : &currentPlayerTeam1;

        for (int i = 0; i < 11; i++) {
            float dx = ballHolder->pos.x - opponentTeam[i].pos.x;
            float dy = ballHolder->pos.y - opponentTeam[i].pos.y;
            float distSq = dx * dx + dy * dy;

            if (distSq < stealDistanceSq) {
                ballHolder = &opponentTeam[i];
                if (isPlayerInTeam1(ballHolder)) {
                    team2[*opponentCurrentPlayerIdx].controlled = false;
                    currentPlayerTeam1 = i;
                    team1[i].controlled = true;
                } else {
                     team1[*opponentCurrentPlayerIdx].controlled = false;
                    currentPlayerTeam2 = i;
                    team2[i].controlled = true;
                }
                possessionCooldown = POSSESSION_COOLDOWN_MS;
                printf("Roubo de bola! Time %d pegou.\n", isPlayerInTeam1(ballHolder) ? 1 : 2);
                return;
            }
        }
    } else {
        for (int i = 0; i < 11; i++) {
             if (touch(ball.pos, team1[i].pos, ball.radius, team1[i].radius + 5.0f)) {
                ballHolder = &team1[i];
                 if (currentPlayerTeam1 != i) {
                     team1[currentPlayerTeam1].controlled = false;
                     currentPlayerTeam1 = i;
                     team1[i].controlled = true;
                 }
                return;
            }
        }
        for (int i = 0; i < 11; i++) {
            if (touch(ball.pos, team2[i].pos, ball.radius, team2[i].radius + 5.0f)) {
                ballHolder = &team2[i];
                 if (currentPlayerTeam2 != i) {
                    team2[currentPlayerTeam2].controlled = false;
                    currentPlayerTeam2 = i;
                    team2[i].controlled = true;
                 }
                return;
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawField();
    drawPlayers(team1, team2);
    drawBall(ball, ballAngle);
    drawScore(scoreLeft, scoreRight);
    glutSwapBuffers();
}

void update() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float dt = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (dt > 0.1f) dt = 0.1f;
    int dt_ms = (int)(dt * 1000.0f);

    if (playerSwitchCooldownTeam1 > 0) playerSwitchCooldownTeam1 -= dt_ms;
    if (playerSwitchCooldownTeam2 > 0) playerSwitchCooldownTeam2 -= dt_ms;
    if (possessionCooldown > 0) possessionCooldown -= dt_ms;

    switchPlayerTeam1();
    switchPlayerTeam2();

    proccesMovement(&team1[currentPlayerTeam1], 1);
    proccesMovement(&team2[currentPlayerTeam2], 2);

    team1[currentPlayerTeam1].pos.x += team1[currentPlayerTeam1].speed.x * dt;
    team1[currentPlayerTeam1].pos.y += team1[currentPlayerTeam1].speed.y * dt;
    team2[currentPlayerTeam2].pos.x += team2[currentPlayerTeam2].speed.x * dt;
    team2[currentPlayerTeam2].pos.y += team2[currentPlayerTeam2].speed.y * dt;

    checkPlayersCollision();

    updateBall(dt);

    tryShoot(&team1[currentPlayerTeam1], 1);
    tryShoot(&team2[currentPlayerTeam2], 2);

    tryPass(team1, &team1[currentPlayerTeam1], 1);
    tryPass(team2, &team2[currentPlayerTeam2], 2);   

    checkBallPossession();

    float ballSpeed = sqrt(ball.speed.x * ball.speed.x + ball.speed.y * ball.speed.y);
    float rotationSpeedFactor = 1.25f;
    if (ballSpeed > SPEED_THRESHOLD) {
        ballAngle += ballSpeed * rotationSpeedFactor * dt * (180.0f / PI);
        if (ballAngle >= 360.0f)
            ballAngle -= 360.0f;
         else if (ballAngle < 0.0f)
             ballAngle += 360.0f;
    }

    glutPostRedisplay();
}

void timerFunc(int value) {
    update();
    glutTimerFunc(8, timerFunc, 0);
}

void keyboardDown(unsigned char key, int x, int y) {
    if(key >= 'A' && key <= 'Z') key += 32;
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    if(key >= 'A' && key <= 'Z') key += 32;
    keys[key] = false;
}

void proccesMovement(Player *player, int team) {
    float targetSpeedX = 0.0f;
    float targetSpeedY = 0.0f;

    if (team == 1) {
        if (keys['w']) targetSpeedY = 1.0f;
        if (keys['s']) targetSpeedY = -1.0f;
        if (keys['d']) targetSpeedX = 1.0f;
        if (keys['a']) targetSpeedX = -1.0f;
    } else {
        if (keys['i']) targetSpeedY = 1.0f;
        if (keys['k']) targetSpeedY = -1.0f;
        if (keys['l']) targetSpeedX = 1.0f;
        if (keys['j']) targetSpeedX = -1.0f;
    }

    float len = sqrt(targetSpeedX * targetSpeedX + targetSpeedY * targetSpeedY);
    if (len > 0.01f) {
        targetSpeedX = (targetSpeedX / len) * MOVE_SPEED;
        targetSpeedY = (targetSpeedY / len) * MOVE_SPEED;
    } else {
        targetSpeedX = 0.0f;
        targetSpeedY = 0.0f;
    }

    player->speed.x = targetSpeedX;
    player->speed.y = targetSpeedY;
}

void init() {
    glClearColor(0.1f, 0.6f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1150, 0, 800);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    lastTime = glutGet(GLUT_ELAPSED_TIME);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    gkTexture = loadTexture("assets/goleiro.png");
    ballTexture = loadTexture("assets/ball.png");
    playerTexture1 = loadTexture("assets/player1.png");
    playerTexture2 = loadTexture("assets/player2.png");

    initPlayers();
    for(int i=0; i<NUM_KEYS; ++i) keys[i] = false;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(2560, 1600);
    glutCreateWindow("Campo de Futebol");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timerFunc, 0);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutMainLoop();
    return 0;
}