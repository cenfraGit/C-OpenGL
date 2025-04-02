#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>

typedef struct {
  float x, y, z;
} Vertex;

typedef struct {
  unsigned int v1, v2, v3;
} Face;

typedef struct {
  float* vertices;
  unsigned int num_vertices;
  unsigned int* indices;
  unsigned int num_indices;
} Mesh;

char* read_shader_source(const char *filename);

Mesh* read_obj(const char *filename);

void render_mesh(Mesh* mesh, unsigned int VAO, unsigned int VBO, unsigned int EBO);

#endif
