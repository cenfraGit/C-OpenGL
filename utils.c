
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "glad/include/glad/glad.h"

// ------------------------------------------------------------
// read shader
// ------------------------------------------------------------

char* read_shader_source(const char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error: failed to open shader source file.\n");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* shader_source = (char*)malloc(file_size + 1);
  if (!shader_source) {
    fprintf(stderr, "Error: failed to allocate memory for shader source.\n");
    fclose(file);
    return NULL;
  }

  size_t read_size = fread(shader_source, 1, file_size, file);
  if (read_size != (size_t)file_size) {
    fprintf(stderr, "Error: failed to read shader file.\n");
    free(shader_source);
    fclose(file);
    return NULL;
  }

  shader_source[file_size] = '\0';
  fclose(file);

  return shader_source;
}

// ------------------------------------------------------------
// read obj
// ------------------------------------------------------------

Mesh* read_obj(const char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error: failed to open obj file.\n");
    return NULL;
  }

  Vertex* vertices = (Vertex*)malloc(1024 * sizeof(Vertex));
  Face* faces = (Face*)malloc(1024 * sizeof(Face));
  unsigned int num_vertices = 0;
  unsigned int num_faces = 0;

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "v ", 2) == 0) {
      Vertex v;
      sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
      vertices[num_vertices++] = v;
    } else if (strncmp(line, "f ", 2) == 0) {
      Face f;
      sscanf(line, "f %u %u %u", &f.v1, &f.v2, &f.v3);
      f.v1--; f.v2--; f.v3--;
      faces[num_faces++] = f;
    }
  }
  fclose(file);

  /* --------- save data to mesh --------- */

  Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
  if (!mesh) {
    fprintf(stderr, "Error: failed to allocate memory for Mesh.\n");
    fclose(file);
    return NULL;
  }

  mesh->num_vertices = num_faces * 3;
  mesh->vertices = (float*)malloc(mesh->num_vertices * 3 * sizeof(float));
  mesh->num_indices = num_faces * 3;
  mesh->indices = (unsigned int*)malloc(mesh->num_indices * sizeof(unsigned int));

  for (unsigned int i = 0; i < num_faces; ++i) {
    Face face = faces[i];
    Vertex v1 = vertices[face.v1];
    Vertex v2 = vertices[face.v2];
    Vertex v3 = vertices[face.v3];

    mesh->vertices[i * 9] = v1.x;
    mesh->vertices[i * 9 + 1] = v1.y;
    mesh->vertices[i * 9 + 2] = v1.z;

    mesh->vertices[i * 9 + 3] = v2.x;
    mesh->vertices[i * 9 + 4] = v2.y;
    mesh->vertices[i * 9 + 5] = v2.z;

    mesh->vertices[i * 9 + 6] = v3.x;
    mesh->vertices[i * 9 + 7] = v3.y;
    mesh->vertices[i * 9 + 8] = v3.z;

    mesh->indices[i * 3] = i * 3;
    mesh->indices[i * 3 + 1] = i * 3 + 1;
    mesh->indices[i * 3 + 2] = i * 3 + 2;
  }

  free(vertices);
  free(faces);
  
  return mesh;
}

void render_mesh(Mesh* mesh, unsigned int VAO, unsigned int VBO, unsigned int EBO) {
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * 3 * sizeof(float), mesh->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);
}
