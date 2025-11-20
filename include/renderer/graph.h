/*
  rgr - Robkoo's Graph Renderer
*/

#ifndef GRAPH_H
#define GRAPH_H

#include <GLFW/glfw3.h>

#define PI 3.14159265358979323846f
// lenght between points on graph
#define GRID_SPACING_NDC          0.1f
#define GRID_SPACING_WORLD        1.0f // one marker per world unit
#define POINT_MARKER_HEIGHT_NDC   0.03f
#define POINT_MARKER_HEIGHT_WORLD 0.3f

/**
  @brief Sets up the graph rendering resources
*/
enum reh_error_code_e rgr_SetupGraph(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO);

/**
  @brief Renders the graph grid lines
*/
enum reh_error_code_e rgr_RenderGraph(GLuint *program, GLuint *VAO, GLuint *VBO, float **projectionMatrixPtr);

/**
  @brief Sets up the marker rendering resources
*/
enum reh_error_code_e rgr_SetupMarkerShaders(GLuint *program);

/**
  @brief Sets up the marker buffers
*/
enum reh_error_code_e rgr_SetupMarkerBuffers(GLuint *VAO, GLuint *VBO, GLuint *EBO);

/**
  @brief Renders the markers
*/
enum reh_error_code_e rgr_RenderMarkers(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO, float **projectionMatrixPtr);

#endif // GRAPH_H
