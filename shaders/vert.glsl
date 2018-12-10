#version 150 core

in vec3 in_position;
in vec2 in_texCoord;
in vec3 in_normal;
in ivec4 in_boneIDs;
in vec4 in_boneWeights;

const vec3 inLightDir = normalize(vec3(0, 1, -1));

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 inColor;

const int MAX_BONES = 100;
uniform mat4 global_bones[MAX_BONES];

out vec3 Color;
out vec3 vertNormal;
out vec3 pos;
out vec3 lightDir;
out vec2 texcoord;

void main() {
    Color = inColor;

    mat4 bonetransform = global_bones[in_boneIDs[0]] * in_boneWeights[0];
    bonetransform     += global_bones[in_boneIDs[1]] * in_boneWeights[1];
    bonetransform     += global_bones[in_boneIDs[2]] * in_boneWeights[2];
    bonetransform     += global_bones[in_boneIDs[3]] * in_boneWeights[3];

    vec4 posTransform = bonetransform * vec4(in_position, 1.0);
    gl_Position = proj * view * model * posTransform;
    pos = (view * model * posTransform).xyz;

//    gl_Position = proj * view * model * vec4(in_position, 1.0);
//    pos = (view * model * vec4(in_position, 1.0)).xyz;


    lightDir = (view * vec4(inLightDir, 0.0)).xyz;

    vec4 norm4 = transpose(inverse(view*model)) * (bonetransform * vec4(in_normal, 0.0));

//    vec4 norm4 = transpose(inverse(view*model)) * vec4(in_normal, 0.0);

    vertNormal = normalize(norm4.xyz);
    texcoord = in_texCoord;
}