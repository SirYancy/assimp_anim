//
// Created by eric on 12/7/18.
//

#ifndef ANIM_TEXTURE_H
#define ANIM_TEXTURE_H

#include <iostream>
#include <string>
#include "../glad/glad.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

class Texture {
private:
    std::string filename;
    std::string samplerID;
    int index;
    GLenum texEnum;
    GLuint texId;

public:
    Texture(GLenum texture, const std::string &filename, const std::string &samplerID, int index);
    bool Load(GLuint shader);
};



#endif //ANIM_TEXTURE_H
