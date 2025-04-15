#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glut.h>

extern GLuint gkTexture;
extern GLuint ballTexture;
extern GLuint playerTexture1;
extern GLuint playerTexture2;

GLuint loadTexture(const char *filename);

#endif