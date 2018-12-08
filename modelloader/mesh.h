//
// Created by eric on 12/7/18.
//

#ifndef ANIM_MESH_H
#define ANIM_MESH_H

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "../glad/glad.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "texture.h"

#define GLM_FORCE_RADIANS

#include "../glm/glm.hpp"

using namespace glm;

struct Vertex
{
    vec3 pos;
    vec2 tex;
    vec3 normal;

    Vertex(const vec3 &pos, const vec2 &tex, const vec3 &normal) : pos(pos), tex(tex), normal(normal){}
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    bool LoadMesh(const std::string &modelfn, GLuint shader, Texture *tex);

    void Render();

private:
    bool InitFromScene(const aiScene *scene);
    bool InitMesh(const aiMesh *inMesh);

    struct MeshData{
         MeshData();
        ~MeshData();

        bool Init(const std::vector<Vertex> &vertices,
                    const std::vector<unsigned int> &indices,
                    GLuint shader);

        GLuint vao; //Vertex array object
        GLuint vbo; //vertex buffer
        GLuint ibo; //index buffer
        unsigned int numIndices;
        unsigned int materialIndex;
    };

    GLuint shader;
    MeshData mesh;
    Texture *texture;

};


#endif //ANIM_MESH_H
