#include "muskat.h"
#include "scene.h"
#include "deamon.h"

void init();
void render();
void move(int v);
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

    Deamon deamon;

    glutReshapeWindow(512, 512);

    muskatRun(render, move);

    deamon.exit();

	return 0;
}

void init() {
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
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void render() {
    ++properties.frameCnt;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 1.0, 1.0);

    renderScene();

    glutSwapBuffers();

    GLubyte* pixels = new GLubyte[properties.width * properties.height * 3];
    glReadPixels(0, 0, properties.width-1, properties.height-1, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    const int JPEG_QUALITY = 75;
    //const int COLOR_COMPONENTS = 3;
    long unsigned int _jpegSize = 0;
    unsigned char* _compressedImage = NULL;

    tjhandle _jpegCompressor = tjInitCompress();

    tjCompress2(_jpegCompressor, pixels, properties.width, 0, properties.height, TJPF_RGB,
              &_compressedImage, &_jpegSize, TJSAMP_444, JPEG_QUALITY,
              TJFLAG_FASTDCT);

    tjDestroy(_jpegCompressor);

    //to free the memory allocated by TurboJPEG (either by tjAlloc(),
    //or by the Compress/Decompress) after you are done working on it:
    //MT_SAFE_DELETE(_compressedImage);
    MT_SAFE_DELETE_ARRAY(pixels);
}

void move(int v) {
    if (0 != v) {
        char* TempString = (char*) malloc(512 + strlen(properties.prefix.c_str()));

        sprintf(TempString, "%s: %d Frames Per Second @ %d x %d", properties.prefix.c_str(),
                                                                  properties.frameCnt * 4,
                                                                  properties.width,
                                                                  properties.height);
        glutSetWindowTitle(TempString);
        free(TempString);
    }

    properties.frameCnt = 0;
    glutTimerFunc(250, move, 1);
}

void exit() {
    destroyScene();
}

void resize(int w, int h) {
    properties.width  = w;
    properties.height = h;
    glViewport(0, 0, w, h);

    matrix_projection = real4x4Projection2(60.0f, ((float)w/(float)h), 1.0f, 100.0f);
    glUseProgram(ShaderIds[0]);
    glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, matrix_projection.n);
    glUseProgram(0);
}
