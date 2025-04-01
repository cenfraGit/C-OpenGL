#include <stdio.h>
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "matrix.h"

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
// shaders
// ------------------------------------------------------------

const char* vertex_shader_source =
  "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;"
  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 projection;"
  "void main() {"
  "  gl_Position = projection * view * model * vec4(aPos, 1.0f);"
  "}";

const char* fragment_shader_source =
  "#version 330 core\n"
  "out vec4 FragColor;"
  "void main() {"
  "FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
  "}\0";

// ------------------------------------------------------------
// variables
// ------------------------------------------------------------

float time_delta = 0.0f;
float last_frame = 0.0f;

// ------------------------------------------------------------
// main
// ------------------------------------------------------------

int main(void) {

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

  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);

  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  /* ------------- gl states ------------- */

  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  /* ---------------- loop ---------------- */

  while (!glfwWindowShouldClose(window)) {
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    float* model = mat4(1.0f);
    rotate_x(model, (float)glfwGetTime());
    rotate_y(model, (float)glfwGetTime());
    rotate_z(model, (float)glfwGetTime());
    
    float* view = mat4(1.0f);
    translate(view, 0.0f, 0.0f, -2.0f);
    
    float* projection = perspective(2, (float)WIDTH / HEIGHT, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_TRUE, model);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_TRUE, view);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_TRUE, projection);

    free(model);
    free(view);
    free(projection);

    glBindVertexArray(VAO);
    glPointSize(3);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
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
