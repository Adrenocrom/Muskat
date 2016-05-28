#ifndef SCENE_H
#define SCENE_H

#pragma once

extern real4x4
    matrix_projection,
    matrix_view,
    matrix_model;

extern GLuint
  ProjectionMatrixUniformLocation,
  ViewMatrixUniformLocation,
  ModelMatrixUniformLocation,
  BufferIds[],
  ShaderIds[];

extern real CubeRotation;
extern clock_t LastTime;

struct Vertex {
   real p[4];
   real c[4];
};

void createScene();
void renderScene();
void destroyScene();

#endif
