//
// Created by eric on 12/7/18.
//

#include "mesh.h"

Mesh::Mesh() {}

Mesh::~Mesh() {
    //TODO
}

bool Mesh::LoadMesh(const std::string &modelfn, GLuint s, Texture *tex) {
    texture = tex;
    shader = s;
    bool success = false;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &posvb);
    glGenBuffers(1, &normvb);
    glGenBuffers(1, &texvb);
    glGenBuffers(1, &bonevb);
    glGenBuffers(1, &indexvb);

    scene = importer.ReadFile(modelfn.c_str(),
                              aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs |
                              aiProcess_JoinIdenticalVertices);

    if (scene) {
        inverseTransform = convertMatrix(scene->mRootNode->mTransformation);
        inverseTransform = glm::inverse(inverseTransform);
        success = InitFromScene(scene);
    } else {
        std::cerr << "Error parsing collada file " << modelfn << importer.GetErrorString() << std::endl;
    }

    glBindVertexArray(0);
    return success;
}

bool Mesh::InitFromScene(const aiScene *scene) {

    uint numIndices = 0;

    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> texcoords;
    vector<VertexBoneData> bones;
    vector<uint> indices;

    uint nVertices = 0;
    uint nIndicies = 0;

    mesh.numIndices = scene->mMeshes[0]->mNumFaces * 3;

    nVertices = scene->mMeshes[0]->mNumVertices;
    nIndicies = mesh.numIndices;

    positions.reserve(nVertices);
    normals.reserve(nVertices);
    texcoords.reserve(nVertices);
    bones.resize(nVertices);
    indices.reserve(nIndicies);

    const aiMesh *pMesh = scene->mMeshes[0];
    InitMesh(pMesh, positions, normals, texcoords, bones, indices);

    glUseProgram(shader);

    // POSITION
    GLint posAttrib = glGetAttribLocation(shader, "in_position");
    glBindBuffer(GL_ARRAY_BUFFER, posvb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0])*positions.size(), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // TEXTURES
    GLint texAttrib = glGetAttribLocation(shader, "in_texCoors");
    glBindBuffer(GL_ARRAY_BUFFER, texvb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords[0])*texcoords.size(), texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // NORMALS
    GLint normAttrib = glGetAttribLocation(shader, "in_normal");
    glBindBuffer(GL_ARRAY_BUFFER, normvb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0])*normals.size(), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // BONESglm rotate with quaternion
    GLint boneIDAttrib = glGetAttribLocation(shader, "in_boneIDs");
    GLint boneWeightAttrib = glGetAttribLocation(shader, "in_boneWeights");
    glBindBuffer(GL_ARRAY_BUFFER, bonevb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0])*bones.size(), bones.data(), GL_STATIC_DRAW);
    glVertexAttribIPointer(boneIDAttrib, 4, GL_INT, sizeof(VertexBoneData), (void*)0);
    glVertexAttribPointer(boneWeightAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)(sizeof(int)*4));

    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(texAttrib);
    glEnableVertexAttribArray(normAttrib);
    glEnableVertexAttribArray(boneIDAttrib);
    glEnableVertexAttribArray(boneWeightAttrib);

    // Generate Index Array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexvb);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*indices.size(), indices.data(), GL_STATIC_DRAW);

    return glGetError() != GL_NO_ERROR;
}

bool Mesh::InitMesh(const aiMesh *inMesh,
                    vector<vec3> &positions,
                    vector<vec3> &normals,
                    vector<vec2> &texCoords,
                    vector<Mesh::VertexBoneData> &bones,
                    vector<unsigned int> &indices) {
    const aiVector3D noVec(0.f, 0.f, 0.f);

    for (int i = 0; i < inMesh->mNumVertices; i++) {
        const aiVector3D *pPos = &(inMesh->mVertices[i]);
        const aiVector3D *pNormal = &(inMesh->mNormals[i]);
        const aiVector3D *pTex = inMesh->HasTextureCoords(0) ? &(inMesh->mTextureCoords[0][i]) : &noVec;

        positions.push_back(vec3(pPos->x, pPos->y, pPos->z));
        normals.push_back(vec3(pNormal->x, pNormal->y, pNormal->z));
        texCoords.push_back(vec2(pTex->x, pTex->y));
    }

    LoadBones(inMesh, bones);

    for(int i = 0; i < inMesh->mNumFaces; i++){
        const aiFace &face = inMesh->mFaces[i];
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
}

void Mesh::LoadBones(const aiMesh *inMesh, vector<Mesh::VertexBoneData> &bones) {
    for(uint i = 0; i < inMesh->mNumBones; i++){
        uint index = 0;
        string name(inMesh->mBones[i]->mName.data);

        if(boneMapping.find(name) == boneMapping.end()){
            index = numBones;
            numBones++;
            BoneInfo info;
            boneInfor.push_back(info);
            boneInfor[index].boneOffset = convertMatrix(inMesh->mBones[i]->mOffsetMatrix);
            boneMapping[name] = index;
        } else{
            index = boneMapping[name];
        }
        for(uint j = 0; j < inMesh->mBones[i]->mNumWeights; j++){
            uint vID = inMesh->mBones[i]->mWeights[j].mVertexId;
            float weight = inMesh->mBones[i]->mWeights[j].mWeight;
            bones[vID].AddBoneData(index, weight);
        }
    }
}

void Mesh::Render() {
    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

glm::mat4 Mesh::convertMatrix(const aiMatrix4x4 &mat) {
    return {
            mat.a1, mat.b1, mat.c1, mat.d1,
            mat.a2, mat.b2, mat.c2, mat.d2,
            mat.a3, mat.b3, mat.c3, mat.d3,
            mat.a4, mat.b4, mat.c4, mat.d4
    };
}

glm::quat Mesh::convertQuaternion(const aiQuaternion &quat) {
    return {
        quat.w, quat.x, quat.y, quat.z
    };
}

void Mesh::BoneTransform(float seconds, vector<mat4> &transforms) {
    mat4 identity = mat4();

    float ticksPerSecond = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float timeInTicks = seconds * ticksPerSecond;
    float animTime = fmod(timeInTicks, (float)scene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(animTime, scene->mRootNode, identity);

    transforms.resize(numBones);

    for(uint i = 0; i < numBones; i++){
        transforms[i] = boneInfor[i].transform;
    }
}

void Mesh::ReadNodeHeirarchy(float animTime, const aiNode *node, const mat4 &parentTransform) {
    string nodeName(node->mName.data);
    const aiAnimation *animation = scene->mAnimations[0];
    mat4 nodeTransformation(convertMatrix(node->mTransformation));

    const aiNodeAnim *nodeAnim = FindNodeAnim(animation, nodeName);

    if(nodeAnim) {
        // Scaling transformation
        aiVector3D scaling;
        CalcInterpScaling(scaling, animTime, nodeAnim);
        mat4 scalingMat;
        glm::scale(scalingMat, vec3(scaling.x, scaling.y, scaling.z));

        // Rotation Transform
        aiQuaternion quatRotation;
        CalcInterpRotating(quatRotation, animTime, nodeAnim);
        mat4 rotationMat = toMat4(convertQuaternion(quatRotation));

        // Translation Transform
        aiVector3D translation;
        CalcInterpPosition(translation, animTime, nodeAnim);
        mat4 transMat;
        glm::translate(transMat, vec3(translation.x, translation.y, translation.z));

        nodeTransformation = transMat * rotationMat * scalingMat;
    }

    mat4 globalTransformation = parentTransform * nodeTransformation;

    if(boneMapping.find(nodeName) != boneMapping.end()){
        uint boneIndex = boneMapping[nodeName];
        boneInfor[boneIndex].transform = inverseTransform * globalTransformation * boneInfor[boneIndex].boneOffset;
    }

    for(uint i = 0; i < node->mNumChildren; i++){
        ReadNodeHeirarchy(animTime, node->mChildren[i], globalTransformation);
    }
}

const aiNodeAnim *Mesh::FindNodeAnim(const aiAnimation *animation, const string nodeName) {
    for (uint i = 0; i < animation->mNumChannels; i++){
        const aiNodeAnim *node = animation->mChannels[i];

        if(string(node->mNodeName.data) == nodeName){
            return node;
        }
    }
    return nullptr;
}

void Mesh::CalcInterpPosition(aiVector3D &out, float animTime, const aiNodeAnim *nodeAnim) {

    if(nodeAnim->mNumPositionKeys == 1){
        out = nodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint positionIndex = FindPosition(animTime, nodeAnim);
    uint nextIndex = positionIndex + 1;
    assert(nextIndex < nodeAnim->mNumPositionKeys);
    float deltaT = (float)(nodeAnim->mPositionKeys[nextIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime);
    float factor = (animTime - (float)nodeAnim->mPositionKeys[positionIndex].mTime) / deltaT;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiVector3D start = nodeAnim->mPositionKeys[positionIndex].mValue;
    const aiVector3D end = nodeAnim->mPositionKeys[nextIndex].mValue;
    aiVector3D delta = end - start;
    out = start + factor * delta;

}

void Mesh::CalcInterpScaling(aiVector3D &out, float animTime, const aiNodeAnim *nodeAnim) {
    if(nodeAnim->mNumScalingKeys == 1){
        out = nodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint scalingIndex = FindScaling(animTime, nodeAnim);
    uint nextIndex = scalingIndex + 1;
    assert(nextIndex < nodeAnim->mNumScalingKeys);
    float deltaT = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
    float factor = (animTime - (float)nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaT;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiVector3D start = nodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D end = nodeAnim->mScalingKeys[nextIndex].mValue;
    aiVector3D delta = end - start;
    out = start + factor * delta;
}

void Mesh::CalcInterpRotating(aiQuaternion &out, float animTime, const aiNodeAnim *nodeAnim) {

    if(nodeAnim->mNumRotationKeys == 1){
        out = nodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint rotationIndex = FindRotation(animTime, nodeAnim);
    uint nextIndex = rotationIndex + 1;
    assert(nextIndex < nodeAnim->mNumRotationKeys);
    float deltaT = (float)(nodeAnim->mRotationKeys[nextIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
    float factor = (animTime - (float)nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaT;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiQuaternion &start = nodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion &end = nodeAnim->mRotationKeys[nextIndex].mValue;
    aiQuaternion::Interpolate(out, start, end, factor);
    out = out.Normalize();
}

uint Mesh::FindScaling(float animTime, const aiNodeAnim *nodeAnim) {
    assert(nodeAnim->mNumScalingKeys > 0);

    for(uint i = 0; i < nodeAnim->mNumScalingKeys - 1; i++){
        if(animTime < (float)nodeAnim->mScalingKeys[i+1].mTime) {
            return i;
        }
    }
    assert(0);
    return 0;
}

uint Mesh::FindRotation(float animTime, const aiNodeAnim *nodeAnim) {
    assert(nodeAnim->mNumRotationKeys > 0);

    for(uint i = 0; i < nodeAnim->mNumRotationKeys - 1; i++){
        if(animTime < (float)nodeAnim->mRotationKeys[i+1].mTime) {
            return i;
        }
    }
    assert(0);
    return 0;
}

uint Mesh::FindPosition(float animTime, const aiNodeAnim *nodeAnim) {
    for(uint i = 0; i < nodeAnim->mNumPositionKeys - 1; i++){
        if(animTime < (float)nodeAnim->mPositionKeys[i+1].mTime) {
            return i;
        }
    }
    assert(0);
    return 0;
}



void Mesh::VertexBoneData::AddBoneData(uint boneID, float weight) {
    for(uint i = 0; i < sizeof(IDs) / sizeof(IDs[0]); i++){
        if(weights[i] == 0.0){
            IDs[i] = boneID;
            weights[i] = weight;
            return;
        }
    }
    return;
    // Should never happen...I hope.
    assert(0);
}




