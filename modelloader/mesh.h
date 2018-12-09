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
#include <map>

#include "texture.h"

#define GLM_FORCE_RADIANS

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/quaternion.hpp"

using namespace std;
using namespace glm;

class Mesh {
public:
    Mesh();

    ~Mesh();

    bool LoadMesh(const std::string &modelfn, GLuint shader, Texture *tex);

    void Render();

    uint NumBones() const{
        return numBones;
    }

    void BoneTransform(float seconds, vector<mat4> &transforms);

private:
    struct BoneInfo
    {
        mat4 boneOffset;
        mat4 transform;

        BoneInfo()
        {
            boneOffset = mat4();
            transform = mat4();
        }
    };

    struct VertexBoneData{
        uint IDs[4];
        float weights[4];

        void AddBoneData(uint boneID, float weight);
    };

    // Animation Transform Functions
    void CalcInterpScaling(aiVector3D &out, float animTime, const aiNodeAnim *nodeAnim);
    void CalcInterpRotating(aiQuaternion &out, float animTime, const aiNodeAnim *nodeAnim);
    void CalcInterpPosition(aiVector3D &out, float animTime, const aiNodeAnim *nodeAnim);
    uint FindScaling(float animTime, const aiNodeAnim *nodeAnim);
    uint FindRotation(float animTime, const aiNodeAnim *nodeAnim);
    uint FindPosition(float animTime, const aiNodeAnim *nodeAnim);
    const aiNodeAnim *FindNodeAnim(const aiAnimation *animation, const string nodeName);
    void ReadNodeHeirarchy(float animTime, const aiNode *node, const mat4 &parentTransform);

    //Initialization
    bool InitFromScene(const aiScene *scene);
    bool InitMesh(const aiMesh *inMesh,
            vector<vec3> &positions,
            vector<vec3> &normals,
            vector<vec2> &texCoords,
            vector<VertexBoneData> &bones,
            vector<unsigned int> &indices);
    void LoadBones(const aiMesh *inMesh, vector<VertexBoneData> &bones);

    glm::mat4 convertMatrix(const aiMatrix4x4 &mat);
    glm::quat convertQuaternion(const aiQuaternion &quat);

    GLuint vao,
            posvb,
            normvb,
            texvb,
            bonevb,
            indexvb;

    struct MeshData {
        MeshData()
        {
            numIndices = 0;
            baseVert = 0;
            baseIndex = 0;
        }

        unsigned int numIndices;
        unsigned int baseVert;
        unsigned int baseIndex;
    };

    GLuint shader;
    MeshData mesh;
    Texture *texture;

    std::map<std::string, uint> boneMapping;
    uint numBones;
    vector<BoneInfo> boneInfor;
    mat4 inverseTransform;

    const aiScene *scene;
    Assimp::Importer importer;

};


#endif //ANIM_MESH_H
