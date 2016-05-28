#ifndef UTILS_H
#define UTILS_H

#pragma once



void ExitOnGLError(const char* error_message);
GLuint LoadShader(const char* filename, GLenum shader_type);

#endif
