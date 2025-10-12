#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "utils/renderUtils.h"

void setupEBO(GLuint *indices, size_t indicesSize, GLuint *EBO){
  glGenBuffers(1, EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
}

void setupRenderData(float *vertices, size_t verticesSize, GLuint *indices, size_t indicesSize, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  // Generate and bind the Vertex Array Object and Vertex Buffer Object
  glGenVertexArrays(1, VAO);
  glGenBuffers(1, VBO);

  glBindVertexArray(*VAO);

  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

  // Set the vertex attribute pointer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // setup EBO !BEFORE! unbinding VBO (definitely didn't spend hours working out why it didnt work ^_^)
  setupEBO(indices, indicesSize, EBO);

  // Unbind the buffer and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
  glBindVertexArray(0); // Unbind VAO
}

