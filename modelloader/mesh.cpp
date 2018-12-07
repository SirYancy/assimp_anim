//
// Created by eric on 12/7/18.
//

#include "mesh.h"

Mesh::MeshData::MeshData() {
    vb = -1;
    ib = -1;
    numIndices = 0;
    materialIndex = 0;
}

Mesh::MeshData::~MeshData() {
    glDeleteBuffers(1, &vb);
    glDeleteBuffers(1, &ib);
}

bool Mesh::MeshData::Init(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) {
    numIndices = indices.size();

    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indices.data(), GL_STATIC_DRAW);
}

Mesh::Mesh(){}

Mesh::~Mesh(){
}

bool Mesh::LoadMesh(const std::string &modelfn, std::string &texfn, GLuint s) {
    shader = s;
    bool success = false;

    Assimp::Importer importer;

    const aiScene *pScene = importer.ReadFile(modelfn.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if(pScene){
        success = InitFromScene(pScene, modelfn, texfn);
    }
    else{
        std::cerr << "Error parsing collada file " << modelfn << importer.GetErrorString() << std::endl;
    }

    return success;
}

bool Mesh::InitFromScene(const aiScene *scene, const std::string &texfn) {

    const aiMesh* mesh = scene->mMeshes[0];
    InitMesh(mesh);

    return InitMaterials(scene, texfn);
}

void Mesh::InitMesh(const aiMesh *inMesh) {

    mesh.materialIndex = inMesh->mMaterialIndex;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const aiVector3D noVec(0.f, 0.f, 0.f);

    for(unsigned int i = 0; i < inMesh->mNumVertices; i++){
        const aiVector3D *pPos = &(inMesh->mVertices[i]);
        const aiVector3D *pNormal = &(inMesh->mNormals[i]);
        const aiVector3D *pTex = (inMesh->HasTextureCoords(0) ? &(inMesh->mTextureCoords[0][i]) : &noVec);

        Vertex v(vec3(pPos->x, pPos->y, pPos->z),
                vec2(pTex->x, pTex->y),
                vec3(pNormal->x, pNormal->y, pNormal->z));

        vertices.push_back(v);
    }

    for(unsigned int i = 0; i < inMesh->mNumFaces; i++){
        const aiFace &face = inMesh->mFaces[i];
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    mesh.Init(vertices, indices);
}

bool Mesh::InitMaterials(const std::string &texfn, const std::string &samplerID, int i){
    texture = new Texture(GL_TEXTURE_2D, texfn.c_str(), samplerID, i);
    if(!texture->Load(shader)) {
        std::cerr << "Error loading texture " << texfn << std::endl;
        return false;
    }

    return true;
}

void Mesh::Render() {

    //TODO CREATE VAO INSTEAD

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, )

}






