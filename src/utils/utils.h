#pragma once
#include "glad/glad.h"
#include "stdint.h"
#include "mymath/mymath.h"
char *readStringFromFile(char *path);

void glfw_errCbck(int code, char *err);

void gl_printGLError(const uint32_t subject, GLenum pname, char *prefixedMessage);
const uint32_t gl_buildProgram(char *vertPath, char *fragPath);

float *concatFloatArrays(float *a, size_t aCount, float *b, size_t bCount);

Vec3f *colorVecFromEnum(Vec3f *colorVec, int col);
void printFloatArr(float *arr, int n);