//
// Created by eric on 12/7/18.
//

#include "texture.h"

Texture::Texture(GLenum t, const std::string &fn, const std::string &sampler, int i) :
                    target(t), filename(fn), samplerID(sampler), index(i){}

bool Texture::Load(GLuint shader) {
    glUseProgram(shader);

    SDL_Surface *surface = SDL_LoadBMP(filename.c_str());

    if(surface == nullptr){
        std::cerr << "Error loading texture " << SDL_GetError() << std::endl;
        return false;
    }

    glGenTextures(1, &texObject);
    glBindTexture(target, texObject);

    glUniform1i(glGetUniformLocation(shader, samplerID.c_str()), index);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(target, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(target);

    glBindTexture(target, 0);

    SDL_FreeSurface(surface);

    return true;
}

void Texture::bind(GLenum unit) {
    glActiveTexture(unit);
    glBindTexture(target, texObject);
}



