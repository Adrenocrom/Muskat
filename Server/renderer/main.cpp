#include "muskat.h"
#include "scene.h"

void init();
void render();
void resize(int w, int h);
void exit();

int main(int argc, char* argv[]) {
    properties.argc = argc;
    properties.argv = argv;
    properties.width  = 1024;
    properties.height = 768;
    muskatInit(init);
    muskatResize(resize);
    muskatExit(exit);
	muskatRun(render);

	return 0;
}

void init() {
    glGetError();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDepthRange(0.0f, 1.0f);
    ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    ExitOnGLError("ERROR: Could not set OpenGL culling options");

    createScene();
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

    renderScene();

    glutSwapBuffers();
}

void exit() {
    destroyScene();
}

void resize(int w, int h) {
    properties.width  = w;
    properties.height = h;
    glViewport(0, 0, w, h);

    matrix_projection = real4x4Projection2(60, ((real)w/(real)h), 1.0f, 100.0f);
    glUseProgram(ShaderIds[0]);
    glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, matrix_projection);
    glUseProgram(0);
}
