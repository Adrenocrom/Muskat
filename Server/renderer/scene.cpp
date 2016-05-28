#include "muskat.h"
#include "scene.h"


real4x4
    matrix_projection,
    matrix_view,
    matrix_model;

GLuint
  ProjectionMatrixUniformLocation,
  ViewMatrixUniformLocation,
  ModelMatrixUniformLocation,
  BufferIds[3] = {0},
  ShaderIds[3] = {0};


real CubeRotation = 0;
clock_t LastTime  = 0;

void createScene() {
    matrix_model      = real4x4Identity();
    matrix_projection = real4x4Identity();
    matrix_view       = real4x4Translation(real3(0.0, 0.0, -2.0));
                      /*real4x4Camera(real3(0.0, 0.0, -2.0),
                                      real3(0.0, 0.0, 0.0));*/
    matrix_projection = real4x4Projection2(60, (real)properties.width / (real)properties.height, 1.0f, 1000.0f);

    const Vertex VERTICES[8] =
    {
      { { -.5f, -.5f,  .5f, 1 }, { 0, 0, 1, 1 } },
      { { -.5f,  .5f,  .5f, 1 }, { 1, 0, 0, 1 } },
      { {  .5f,  .5f,  .5f, 1 }, { 0, 1, 0, 1 } },
      { {  .5f, -.5f,  .5f, 1 }, { 1, 1, 0, 1 } },
      { { -.5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } },
      { { -.5f,  .5f, -.5f, 1 }, { 1, 0, 0, 1 } },
      { {  .5f,  .5f, -.5f, 1 }, { 1, 0, 1, 1 } },
      { {  .5f, -.5f, -.5f, 1 }, { 0, 0, 1, 1 } }
    };

    const GLuint INDICES[36] =
    {
      0,2,1,  0,3,2,
      4,3,0,  4,7,3,
      4,1,5,  4,0,1,
      3,6,2,  3,7,6,
      1,6,5,  1,2,6,
      7,5,6,  7,4,5
    };

    ShaderIds[0] = glCreateProgram();
    ExitOnGLError("ERROR: Could not create the shader program");
    {
      ShaderIds[1] = LoadShader("fragment.glsl", GL_FRAGMENT_SHADER);
      ShaderIds[2] = LoadShader("vertex.glsl", GL_VERTEX_SHADER);
      glAttachShader(ShaderIds[0], ShaderIds[1]);
      glAttachShader(ShaderIds[0], ShaderIds[2]);
    }
    glLinkProgram(ShaderIds[0]);
    ExitOnGLError("ERROR: Could not link the shader program");

    ModelMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
    ViewMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
    ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
    ExitOnGLError("ERROR: Could not get shader uniform locations");

    glGenVertexArrays(1, &BufferIds[0]);
    ExitOnGLError("ERROR: Could not generate the VAO");
    glBindVertexArray(BufferIds[0]);
    ExitOnGLError("ERROR: Could not bind the VAO");

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    ExitOnGLError("ERROR: Could not enable vertex attributes");

    glGenBuffers(2, &BufferIds[1]);
    ExitOnGLError("ERROR: Could not generate the buffer objects");

    glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
    ExitOnGLError("ERROR: Could not bind the VBO to the VAO");

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].p));
    ExitOnGLError("ERROR: Could not set VAO attributes");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
    ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

    glBindVertexArray(0);
}

void renderScene() {
    real CubeAngle;
    clock_t Now = clock();

    if (LastTime == 0)
      LastTime = Now;

    CubeRotation += 45.0f * ((real)(Now - LastTime) / CLOCKS_PER_SEC);
    CubeAngle = SI_DEG_TO_RAD(CubeRotation);
    LastTime = Now;

    matrix_model  = real4x4Identity();
    //matrix_model *= real4x4Scaling(real3(2.0, 2.0, 2.0));
    matrix_model *= real4x4RotationY(CubeAngle);
    matrix_model *= real4x4RotationX(CubeAngle);

    glUseProgram(ShaderIds[0]);
    ExitOnGLError("ERROR: Could not use the shader program");

    glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, matrix_model);
    glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, matrix_view);
    ExitOnGLError("ERROR: Could not set the shader uniforms");

    glBindVertexArray(BufferIds[0]);
    ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
    ExitOnGLError("ERROR: Could not draw the cube");

    glBindVertexArray(0);
    glUseProgram(0);
}

void destroyScene() {
    glDetachShader(ShaderIds[0], ShaderIds[1]);
    glDetachShader(ShaderIds[0], ShaderIds[2]);
    glDeleteShader(ShaderIds[1]);
    glDeleteShader(ShaderIds[2]);
    glDeleteProgram(ShaderIds[0]);
    ExitOnGLError("ERROR: Could not destroy the shaders");

    glDeleteBuffers(2, &BufferIds[1]);
    glDeleteVertexArrays(1, &BufferIds[0]);
    ExitOnGLError("ERROR: Could not destroy the buffer objects");
}
