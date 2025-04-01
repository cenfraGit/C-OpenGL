#ifndef _MATRIX_H
#define _MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define 3.141592653589793
#endif

#define radians(value) ((value)*M_PI / 180.0)

float* mat4(float n) {
  float* matrix = (float*)malloc(16 * sizeof(float));
  if (!matrix) {
    fprintf(stderr, "Error: failure initializing matrix.");
    return NULL;
  }
  for (int i = 0; i < 16; i++) {
    matrix[i] = 0.0f;
  }
  matrix[0] = n;
  matrix[5] = n;
  matrix[10] = n;
  matrix[15] = n;
  return matrix;
}

void mat4_print(float* matrix) {
  for (int i = 0; i < 16; i=i+4) {
    printf("%.3f %.3f %.3f %.3f\n", matrix[i], matrix[i+1], matrix[i+2], matrix[i+3]);
  }
  printf("\n");
}

// matrix multiplication. applied and saved to M1 (destructive)
void mat4_multiply(float* M1, float* M2) {
  float temp_result;
  float* result = mat4(0.0f);
  for (int rowM1 = 0; rowM1 < 4; rowM1++)
    for (int colM2 = 0; colM2 < 4; colM2++) {
      temp_result = 0.0; // reset
      // multiply m1 row by m2 col
      for (int colM1 = 0, rowM2 = 0; colM1 < 4; colM1++, rowM2++)
	temp_result += *(M1 + (4 * rowM1) + colM1) * *(M2 + (4 * rowM2) + colM2);
      *(result + (4 * rowM1) + colM2) = temp_result;
    }
  for (int i = 0; i < 16; i++){
    M1[i] = result[i];
  }
  free(result);
}

// scaling function for mat4 (destructive)
void scale(float* matrix, float s) {
  // create scaling matrix
  float* matrix_scaling = mat4(s);
  matrix_scaling[15] = 1.0f;
  // apply matrix
  mat4_multiply(matrix, matrix_scaling);
}

void translate(float* matrix, float tx, float ty, float tz) {
  // create translation matrix
  float* matrix_translation = mat4(1.0f);
  matrix_translation[3] = tx;
  matrix_translation[7] = ty;
  matrix_translation[11] = tz;
  // apply matrix
  mat4_multiply(matrix, matrix_translation);
}

void rotate_x(float* matrix, float angle) {
  // create rotation matrix
  float* matrix_rotation = mat4(1.0f);
  matrix_rotation[5] = cosf(angle);
  matrix_rotation[6] = -sinf(angle);
  matrix_rotation[9] = sinf(angle);
  matrix_rotation[10] = cosf(angle);
  // apply matrix
  mat4_multiply(matrix, matrix_rotation);
}

void rotate_y(float* matrix, float angle) {
  // create rotation matrix
  float* matrix_rotation = mat4(1.0f);
  matrix_rotation[0] = cosf(angle);
  matrix_rotation[2] = sinf(angle);
  matrix_rotation[8] = -sinf(angle);
  matrix_rotation[10] = cosf(angle);
  // apply matrix
  mat4_multiply(matrix, matrix_rotation);
}

void rotate_z(float* matrix, float angle) {
  // create rotation matrix
  float* matrix_rotation = mat4(1.0f);
  matrix_rotation[0] = cosf(angle);
  matrix_rotation[1] = -sinf(angle);
  matrix_rotation[4] = sinf(angle);
  matrix_rotation[5] = cosf(angle);
  // apply matrix
  mat4_multiply(matrix, matrix_rotation);
}

float* perspective(float fov, float aspect_ratio, float near, float far) {
  float* matrix = mat4(0.0f);
  float f = 1.0f / tanf(fov / 2);
  matrix[0] = f / aspect_ratio;
  matrix[5] = f;
  matrix[10] = (far + near) / (near - far);
  matrix[11] = (2 * far * near) / (near - far);
  matrix[14] = -1;
  return matrix;
}

#endif
