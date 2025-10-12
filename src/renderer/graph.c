#include "core/logger.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "renderer/graph.h"
#include "utils/renderUtils.h"
#include "utils/shaderUtils.h"

#include <stdlib.h>

void setupGraph(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  // get shader sources
  const char* vertexShaderSrc = loadShaderSource("data/shaders/lineRender.vert");
  const char* fragmentShaderSrc = loadShaderSource("data/shaders/basicColor.frag");

  // make sure we didnt get NULL
  if (vertexShaderSrc == nullptr || fragmentShaderSrc == nullptr){
    logMsg(ERROR, "%s in 'setupGraph()' is NULL.", (vertexShaderSrc == NULL) ? "vertexShaderSrc" : "fragmentShaderSrc");
    return;
  }

  // setup the program
  GLuint vertexShader, fragShader;
  vertexShader = compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
  fragShader = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);

  if (vertexShader == 0 || fragShader == 0){
    logMsg(ERROR, "Compilation of %s in 'setupGraph()' failed.", (vertexShader == 0) ? "vertexShader" : "fragmentShader");
    return; 
  }

  *program = linkShaders(vertexShader, fragShader);

  float vertices[] = {
    // x axis
    X_MIN, 0.0f, 0.0f,
    X_MAX, 0.0f, 0.0f,

    // y axis
    0.0f, Y_MIN, 0.0f,
    0.0f, Y_MAX, 0.0f
  };

  GLuint indices[] = {
    0, 1, // x axis
    2, 3  // y axis
  };

  size_t verticesSize = sizeof(vertices);
  size_t indicesSize = sizeof(indices);

  setupRenderData(vertices, verticesSize, indices, indicesSize, VAO, VBO, EBO);

  free((void*)vertexShaderSrc);
  free((void*)fragmentShaderSrc);
}

void renderGraph(GLuint *program, GLuint *VAO){
  if (program == nullptr){
    logMsg(ERROR, "Shader program passed to 'renderGraph()' is NULL.");
    return;
  }
  if (VAO == nullptr || *VAO == 0){
    logMsg(ERROR, "Invalid VAO passed to 'renderGraph()' (%s).", (VAO == nullptr) ? "NULL" : "0");
    return;
  }

  glUseProgram(*program);
  glBindVertexArray(*VAO);
  gluSet4f(*program, "color", 1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(1.0f);
  glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void setupMarkers(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  // get shader sources
  char* vertexShaderSrc = loadShaderSource("data/shaders/lineRender.vert");
  char* fragmentShaderSrc = loadShaderSource("data/shaders/basicColor.frag");

  // make sure we didnt get NULL
  if (vertexShaderSrc == nullptr || fragmentShaderSrc == nullptr){
    logMsg(ERROR, "%s in 'setupMarkers()' is NULL.", (vertexShaderSrc == NULL) ? "vertexShaderSrc" : "fragmentShaderSrc");
  }

  // setup the program
  GLuint vertexShader, fragShader;
  vertexShader = compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
  fragShader = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
 
  if (vertexShader == 0 || fragShader == 0){
    logMsg(ERROR, "Compilation of %s in 'setupGraph()' failed.", (vertexShader == 0) ? "vertexShader" : "fragmentShader");
    return; 
  }

  *program = linkShaders(vertexShader, fragShader);
 
  float step = POINT_DISTANCE;

  // holds all the marker data (2 points, each point 3 floats)
  float vertices[(X_COUNT + Y_COUNT) * 3 * 2];

  // populate the vertices array
  int v = 0;

  for (int i = 0;i < X_COUNT; ++i){
    float x = X_MIN + i * step;
    // first point of the marker
    vertices[v++] = x;
    vertices[v++] = 0.0f + POINT_MARKER_HEIGHT;
    vertices[v++] = 0.0f;

    vertices[v++] = x;
    vertices[v++] = 0.0f - POINT_MARKER_HEIGHT;
    vertices[v++] = 0.0f;
  }

  for (int i = 0;i < Y_COUNT; ++i){
    float y = Y_MIN + i * step;
    // first point of the marker
    vertices[v++] = 0.0f + POINT_MARKER_HEIGHT;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    vertices[v++] = 0.0f - POINT_MARKER_HEIGHT;
    vertices[v++] = y;
    vertices[v++] = 0.0f;
  }

  // populate the indices array
  GLuint indices[(TOTAL_MARKERS * 2)];

  for (int k = 0; k < TOTAL_MARKERS * 2; ++k) indices[k] = k;

  size_t verticesSize = sizeof(vertices);
  size_t indicesSize = sizeof(indices);

  setupRenderData(vertices, verticesSize, indices, indicesSize, VAO, VBO, EBO);

  free(vertexShaderSrc);
  free(fragmentShaderSrc);
}

void renderMarkers(GLuint *program, GLuint *VAO){
  if (program == nullptr){
    logMsg(ERROR, "Shader program passed to 'renderMarkers()' is NULL.");
    return;
  }
  if (VAO == nullptr || *VAO == 0){
    logMsg(ERROR, "Invalid VAO passed to 'renderMarkers()' (%s).", (VAO == nullptr) ? "NULL" : "0");
    return;
  }

  int indexCount = TOTAL_MARKERS * 2;

  glUseProgram(*program);
  glBindVertexArray(*VAO);
  gluSet4f(*program, "color", 1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);
  glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
