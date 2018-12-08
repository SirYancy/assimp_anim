//
// Created by eric on 12/7/18.
//

#include "mesh.h"

Mesh::MeshData::MeshData() {
    vao = -1;
    vbo = -1;
    ibo = -1;
    numIndices = 0;
    materialIndex = 0;
}

Mesh::MeshData::~MeshData() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

bool Mesh::MeshData::Init(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, GLuint shader) {
    numIndices = indices.size();

    glUseProgram(shader);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(shader, "in_position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

    GLint texAttrib = glGetAttribLocation(shader, "in_texCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));

    GLint normAttrib = glGetAttribLocation(shader, "in_normal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (5 * sizeof(float)));

    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(texAttrib);
    glEnableVertexAttribArray(normAttrib);

    // Generate Index Array
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indices.data(), GL_STATIC_DRAW);

}

Mesh::Mesh(){}

Mesh::~Mesh(){
}

bool Mesh::LoadMesh(const std::string &modelfn, const std::string &texfn, GLuint s) {
    shader = s;
    bool success = false;

    Assimp::Importer importer;

    const aiScene *pScene = importer.ReadFile(modelfn.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if(pScene){
        success = InitFromScene(pScene, texfn);
    }
    else{
        std::cerr << "Error parsing collada file " << modelfn << importer.GetErrorString() << std::endl;
    }

    return success;
}

bool Mesh::InitFromScene(const aiScene *scene, const std::string &texfn) {

    const aiMesh* mesh = scene->mMeshes[0];
    InitMesh(mesh);

    std::string::size_type dot = texfn.find_last_of('.');
    std::string::size_type slash = texfn.find_last_of('/');

    std::string samplerID = texfn.substr(slash + 1, dot - slash - 1);

    return InitMaterials(texfn, samplerID, 1);
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

    mesh.Init(vertices, indices, shader);
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
    glBindVertexArray(mesh.vao);

    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
}






