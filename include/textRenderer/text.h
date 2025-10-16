/*
  id of these functions is 'rtr_..' which stands for Robkoo's Text Renderer
*/

#ifndef TEXT_H
#define TEXT_H

// FT documentation recommended way of including FT
#include <GL/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdint.h>

#include "math/Vec2.h"
#include "math/Vec3.h"
#include "core/errorHandler.h"

#define ASCII_CHAR_COUNT 128

struct rtr_character_t {
  FT_UInt           textureID;  // ID handle of the glyph texture
  struct rm_vec2_t  size;       // Size of glyph
  struct rm_vec2_t  bearing;    // Offset from baseline to left/top of the glyph
  FT_UInt           advance;    // Offset to advance to the next glyph
};

enum reh_error_code_e rtr_initFt(FT_Library *library);
enum reh_error_code_e rtr_initFtFace(FT_Library *library, FT_Face *face);
enum reh_error_code_e rtr_loadChar(FT_Face face, uint8_t ch);
enum reh_error_code_e rtr_loadCharactersIntoArray(FT_Face face, struct rtr_character_t *characters);
enum reh_error_code_e rtr_createTextRenderVAO(GLuint *VAO, GLuint *VBO);
enum reh_error_code_e rtr_renderText(GLuint program, GLuint VAO, GLuint VBO, const char *text, struct rtr_character_t *characters, float x, float y, float scale, struct rm_vec3_t color);

// label rendering
enum reh_error_code_e rtr_calculateTextWidth(const char *text, struct rtr_character_t *characters, float scale, float *totalWidth);
enum reh_error_code_e rtr_calculateTextHeight(const char *text, struct rtr_character_t *characters, float scale, float *totalHeight, float *ascent);
enum reh_error_code_e rtr_formatMarkerValue(float value, char* buffer, const int bufferSize);
float rtr_ndcToPixelX(float ndcX);
float rtr_ndcToPixelY(float ndcY);
enum reh_error_code_e rtr_renderAxisLabels(GLuint program, GLuint VAO, GLuint VBO, struct rtr_character_t *characters, float scale, struct rm_vec3_t color);
#endif // TEXT_H
