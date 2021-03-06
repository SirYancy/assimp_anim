#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "glad/glad.h"
#include "modelloader/mesh.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

using namespace std;
using std::ifstream;

bool fullscreen = false;
int screenWidth = 800;
int screenHeight = 600;

const char *modelfn = "../models/ninja.dae";
const char *texfn = "../textures/ninja.bmp";

const char *vertfn = "../shaders/vert.glsl";
const char *fragfn = "../shaders/frag.glsl";

GLuint shaderProgram;

void CreateShaderProgram(GLuint vertShader, GLuint fragShader);

int LoadShader(const char *filename, GLuint shaderID);

#define GLM_FORCE_RADIANS

#include "glm/gtc/type_ptr.hpp"

int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_VIDEO);

    //Print the version of SDL we are using
    SDL_version comp;
    SDL_version linked;
    SDL_VERSION(&comp);
    SDL_GetVersion(&linked);
    printf("\nCompiled against SDL version %d.%d.%d\n", comp.major, comp.minor, comp.patch);
    printf("Linked SDL version %d.%d.%d.\n", linked.major, linked.minor, linked.patch);

    //Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    //Create a window (offsetx, offsety, width, height, flags)
    SDL_Window *window = SDL_CreateWindow("My OpenGL Program", 100, 100,
                                          screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    if (!window) {
        printf("Could not create window: %s\n", SDL_GetError());
        return EXIT_FAILURE; //Exit as SDL failed
    }
    float aspect = screenWidth / (float) screenHeight; //aspect ratio needs update on resize

    SDL_GLContext context = SDL_GL_CreateContext(window); //Bind OpenGL to the window

    if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        printf("OpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n", glGetString(GL_VERSION));
    } else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }

    GLuint vertShader;
    GLuint fragShader;
    CreateShaderProgram(vertShader, fragShader);

    Texture *t = new Texture(GL_TEXTURE0, texfn, "charTex", 0);
    t->Load(shaderProgram);

    Mesh *m = new Mesh();
    m->LoadMesh(modelfn, shaderProgram, t);

    glEnable(GL_DEPTH_TEST);

    SDL_Event windowEvent;
    bool isRunning = false;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) quit = true;
            if (windowEvent.type == SDL_KEYDOWN){
                switch(windowEvent.key.keysym.sym){
                    case SDLK_w:
                        isRunning = true;
                        break;
                    default:
                        break;
                }
            }
            if (windowEvent.type == SDL_KEYUP) {
                switch (windowEvent.key.keysym.sym) {
                    case SDLK_w:
                        isRunning = false;
                        break;
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        quit = true;
                        break;
                    default:
                        break;
                }
            }
        }

        float currentFrame = SDL_GetTicks() / 1000.f;

        glClearColor(0.2f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::vec3 colVec(0.f, 0.7f, 0.f);

        glm::mat4 model;

        glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, -2.f, -4.5f),
                glm::vec3(0.0f, 0.0f, -4.5f),
                glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 proj = glm::perspective(3.14f / 4, aspect, 0.001f, 1000.0f);

        m->Render(currentFrame, model, view, proj, isRunning);

        SDL_GL_SwapWindow(window);
    }
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragShader);
    glDeleteShader(vertShader);

    SDL_GL_DeleteContext(context);
    SDL_Quit();

    return 0;
}

void CreateShaderProgram(GLuint vertShader, GLuint fragShader) {

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    LoadShader(vertfn, vertShader);
    LoadShader(fragfn, fragShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

}

int LoadShader(const char *filename, GLuint shaderID) {
    ifstream file;
    file.open(filename, std::ios::in);
    if (!file) return -1;

    file.tellg();
    file.seekg(0, std::ios::end);
    int len = static_cast<int>(file.tellg());
    file.seekg(std::ios::beg);

    if (len == 0) return -1; //empty file

    GLchar *shaderSource = new GLchar[len + 1];
    shaderSource[len] = 0;

    int i = 0;
    while (file.good()) {
        shaderSource[i] = static_cast<GLchar>(file.get());
        if (!file.eof())
            i++;
    }

    shaderSource[i] = 0;
    file.close();

    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buffer[512];
        glGetShaderInfoLog(shaderID, 512, NULL, buffer);
        printf("Shader Compile Failed. Info:\n\n%s\n", buffer);
        return -1;
    }
    return 0;
}