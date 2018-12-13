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

#define BONES_PER_VERTEX 4

class Mesh {
public:
    Mesh();

    ~Mesh();

    bool LoadMesh(const std::string &modelfn, GLuint shader, Texture *tex);

    void Render(float currentFrame, glm::mat4 model, glm::mat4 view, glm::mat4 proj, bool running);

    uint NumBones() const{
        return numBones;
    }

    void BoneTransform(float seconds, vector<mat4> &transforms);

private:
    struct BoneInfo
    {
        aiMatrix4x4 boneOffset;
        aiMatrix4x4 finalTransform;

        BoneInfo()
        {
            boneOffset = aiMatrix4x4();
            finalTransform = aiMatrix4x4();
        }
    };

    struct VertexBoneData{
        uint IDs[BONES_PER_VERTEX]{};
        float weights[BONES_PER_VERTEX]{};

        VertexBoneData()
        {
            memset(IDs, 0, sizeof(IDs));
            memset(weights, 0, sizeof(weights));
        }

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
    void ReadNodeHierarchy(float animTime, const aiNode *node, const aiMatrix4x4 &parentTransform);

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

    GLuint vao,
            posvb,
            normvb,
            texvb,
            bonevb,
            indexvb;

    GLint boneLocations[100];

    struct MeshData {
        MeshData()
        {
            numIndices = 0;
        }

        unsigned int numIndices;
    };

    GLuint shader;
    MeshData mesh;
    Texture *texture;

    std::map<std::string, uint> boneMapping;
    uint numBones;
    vector<BoneInfo> boneInfo;
    aiMatrix4x4 inverseTransform;

    const aiScene *scene;
    Assimp::Importer importer;

};


#endif //ANIM_MESH_H
