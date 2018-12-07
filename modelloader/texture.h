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


class Texture {
private:
    std::string filename;
    std::string samplerID;
    int index;
    GLenum target;
    GLuint texObject;

public:
    Texture(GLenum t, const std::string &fn, const std::string &sampler, int i);
    bool Load(GLuint shader);
    void bind(GLenum unit);
};



#endif //ANIM_TEXTURE_H
