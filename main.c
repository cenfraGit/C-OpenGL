#include <stdio.h>
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "matrix.h"
#include "utils.h"

#define WIDTH 1500
#define HEIGHT 900

// ------------------------------------------------------------
// prototypes
// ------------------------------------------------------------

void callback_framebuffer_size(GLFWwindow* window, int width, int height);
void callback_mouse(GLFWwindow* window, double xpos, double ypos);
void callback_scroll(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);

// ------------------------------------------------------------
// variables
// ------------------------------------------------------------

float time_delta = 0.0f;
float last_frame = 0.0f;

typedef struct {
  float x, y, z;
  float fov;
  float aspect_ratio;
  float near;
  float far;
  float transform[16];
} Camera;

typedef struct {
  float transform[16];
} Object3D;

void init_transform(void* ptr, int type) {
  float* identity = mat4(1.0f);
  if (type == 0) {
    Camera* camera = (Camera*)ptr;
    for (int i = 0; i < 16; i++) {
      camera->transform[i] = identity[i];
    }
  }
  else if (type == 1) {
    Object3D* object = (Object3D*)ptr;
    for (int i = 0; i < 16; i++) {
      object->transform[i] = identity[i];
    }
  }
  free(identity);
}

void set_position(void* ptr, int type, float x, float y, float z) {
  if (type == 0) {
    Camera* camera = (Camera*)ptr;
    camera->transform[3]  = x;
    camera->transform[7]  = y;
    camera->transform[11] = z;
  }
  else if (type == 1) {
    Object3D* object = (Object3D*)ptr;
    object->transform[3]  = x;
    object->transform[7]  = y;
    object->transform[11] = z;
  }
}

Camera camera;

// ------------------------------------------------------------
// main
// ------------------------------------------------------------

int main(void) {

  /* --------------- camera --------------- */

  camera.x = 0.0f;
  camera.y = 0.0f;
  camera.z = -2.0f;
  camera.fov = radians(90.0f);
  camera.aspect_ratio = (float)WIDTH/HEIGHT;
  camera.near = 0.01;
  camera.far = 100.0f;
  init_transform(&camera, 0);
  set_position(&camera, 0, camera.x, camera.y, camera.z);

  /* ---------------- glfw ---------------- */

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Error: failed to create glfw window.\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, callback_framebuffer_size);
  glfwSetCursorPosCallback(window, callback_mouse);
  glfwSetScrollCallback(window, callback_scroll);
  /* glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); */

  /* ------------- init glad ------------- */

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "Error: failed to initialize glad.\n");
    return -1;
  }

  /* -------------- program -------------- */

  const char* source_vertex = read_shader_source("vertex.glsl");
  const char* source_fragment = read_shader_source("fragment.glsl");

  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &source_vertex, NULL);
  glCompileShader(vertex_shader);

  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &source_fragment, NULL);
  glCompileShader(fragment_shader);

  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  Mesh* cube = read_obj("dod.obj");

  /* unsigned int VBO, VAO; */
  /* glGenVertexArrays(1, &VAO); */
  /* glGenBuffers(1, &VBO); */
  /* glBindVertexArray(VAO); */
  /* glBindBuffer(GL_ARRAY_BUFFER, VBO); */
  /* glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); */
  /* glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); */
  /* glEnableVertexAttribArray(0); */
  
  /* ------------- gl states ------------- */

  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  /* ---------------- loop ---------------- */

  unsigned int VAO, VBO, EBO;
  if (cube) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  }

  while (!glfwWindowShouldClose(window)) {

    float current_frame = glfwGetTime();
    time_delta = current_frame - last_frame;
    last_frame = current_frame;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    float* model = mat4(1.0f);
    rotate_x(model, (float)glfwGetTime());
    rotate_y(model, (float)glfwGetTime());
    rotate_z(model, (float)glfwGetTime());

    set_position(&camera, 0, camera.x, camera.y, camera.z);
    float* view = camera.transform;
    float* projection = perspective(camera.fov, camera.aspect_ratio, camera.near, camera.far);

    glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_TRUE, model);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_TRUE, view);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_TRUE, projection);

    free(model);
    free(projection);

    render_mesh(cube, VAO, VBO, EBO);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    process_input(window);
  }
  glfwTerminate();
  return 0;
}

// ------------------------------------------------------------
// input
// ------------------------------------------------------------

void process_input(GLFWwindow* window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
  float amount = 3.0f * time_delta;
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.z += amount;
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.x += amount;
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.z -= amount;
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.x -= amount;
}

// ------------------------------------------------------------
// callbacks
// ------------------------------------------------------------

void callback_framebuffer_size(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void callback_mouse(GLFWwindow* window, double xposIn, double yposIn) {
}

void callback_scroll(GLFWwindow* window, double xoffset, double yoffset) {
}
