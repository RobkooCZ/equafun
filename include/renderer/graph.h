#ifndef GRAPH_H
#define GRAPH_H

#include <GLFW/glfw3.h>

// lenght between points on graph
#define GRID_SPACING_NDC          0.1f
#define GRID_SPACING_WORLD        1.0f  // one marker per world unit
#define POINT_MARKER_HEIGHT_NDC   0.03f
#define POINT_MARKER_HEIGHT_WORLD 0.3f
// prototypes

enum reh_error_code_e setupGraph(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO);
enum reh_error_code_e renderGraph(GLuint *program, GLuint *VAO, GLuint *VBO, float **projectionMatrixPtr);

enum reh_error_code_e setupMarkerShaders(GLuint *program);
enum reh_error_code_e setupMarkerBuffers(GLuint *VAO, GLuint *VBO, GLuint *EBO);
enum reh_error_code_e renderMarkers(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO, float **projectionMatrixPtr);

#endif // GRAPH_H
