#version 150 core

in vec3 in_position;
in vec3 in_normal;
in vec2 in_texCoord;

const vec3 inLightDir = normalize(vec3(0, 1, -1));

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 inColor;

out vec3 Color;
out vec3 vertNormal;
out vec3 pos;
out vec3 lightDir;
out vec2 texcoord;

void main() {
    Color = inColor;
    gl_Position = proj * view * model * vec4(in_position, 1.0);
    pos = (view * model * vec4(in_position, 1.0)).xyz;
    lightDir = (view * vec4(inLightDir, 0.0)).xyz;

    vec4 norm4 = transpose(inverse(view*model)) * vec4(in_normal, 0.0);
    vertNormal = normalize(norm4.xyz);
    texcoord = in_texCoord;
}