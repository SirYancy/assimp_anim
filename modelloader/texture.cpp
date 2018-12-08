//
// Created by eric on 12/7/18.
//

#include "texture.h"

Texture::Texture(GLenum e, const std::string &fn, const std::string &sid, const int i):
                texEnum(e), filename(fn), samplerID(sid), index(i) {}

bool Texture::Load(GLuint shader) {
    glUseProgram(shader);

    SDL_Surface *surface = IMG_Load(filename.c_str());

    if(surface == nullptr){
        std::cerr << "Error loading texture " << SDL_GetError() << std::endl;
        return false;
    }

    int mode = GL_RGB;

    if(surface->format->BytesPerPixel == 4){
        mode = GL_RGBA;
    }

    glGenTextures(1, &texId);

    glActiveTexture(texEnum);
    glBindTexture(GL_TEXTURE_2D, texId);

    glUniform1i(glGetUniformLocation(shader, samplerID.c_str()), index);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);
    return true;
}
