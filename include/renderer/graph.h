#ifndef GRAPH_H
#define GRAPH_H

#include <GLFW/glfw3.h>

// x and y axis 'range' on the screen
#define Y_MIN -0.9f
#define Y_MAX  0.9f

#define X_MIN -0.9f
#define X_MAX  0.9f

// color
#define COLOR (1.0f, 1.0f, 1.0f, 1.0f) // white with full alpha

// lenght between points on graph
#define POINT_DISTANCE 0.05f
#define POINT_MARKER_HEIGHT 0.02f

// for ease of use
#define X_COUNT (int)((X_MAX - X_MIN) / POINT_DISTANCE + 1)
#define Y_COUNT (int)((Y_MAX - Y_MIN) / POINT_DISTANCE + 1)
#define TOTAL_MARKERS (X_COUNT + Y_COUNT)

// prototypes
GLuint graphVertexInput(void);

void setupGraph(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO);
void renderGraph(GLuint *program, GLuint *VAO);

void setupMarkers(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO);
void renderMarkers(GLuint *program, GLuint *VAO);

#endif // GRAPH_H
