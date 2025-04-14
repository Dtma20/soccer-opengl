#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GLES3/gl3.h>

GLuint ballTexture;
GLuint playerTexture1;
GLuint playerTexture2;

GLuint loadTexture(const char* filename) {
    stbi_set_flip_vertically_on_load(1);
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
  
    if (!data) {
      fprintf(stderr, "Failed to load texture %s\n", filename);
      return -1;
    }

    GLuint textureId;
  
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
    if (channels == 3) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
    } else if (channels == 4) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data);
    }
  
    glGenerateMipmap(GL_TEXTURE_2D);
  
    stbi_image_free(data);
  
    return textureId;
  }
    
