/*
  rtr - Robkoo's Text Renderer
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
  FT_UInt           textureID;  /**< ID handle of the glyph texture */
  struct rm_vec2_t  size;       /**< Size of glyph */
  struct rm_vec2_t  bearing;    /**< Offset from baseline to left/top of the glyph */
  FT_UInt           advance;    /**< Offset to advance to the next glyph */
};

/**
  @brief Initializes FreeType library
*/
enum reh_error_code_e rtr_InitFt(FT_Library *library);

/**
  @brief Initializes FreeType face
*/
enum reh_error_code_e rtr_InitFtFace(FT_Library *library, FT_Face *face);

/**
  @brief Loads a single character glyph into a texture
*/
enum reh_error_code_e rtr_LoadChar(FT_Face face, uint8_t ch);

/**
  @brief Loads all ASCII characters into an array of struct rtr_character_t
*/
enum reh_error_code_e rtr_LoadCharactersIntoArray(FT_Face face, struct rtr_character_t *characters);

/**
  @brief Creates VAO and VBO for text rendering
*/
enum reh_error_code_e rtr_CreateTextRenderVAO(GLuint *VAO, GLuint *VBO);

/**
  @brief Renders text at the specified position, scale, and color
*/
enum reh_error_code_e rtr_RenderText(GLuint program, GLuint VAO, GLuint VBO, const char *text, struct rtr_character_t *characters, float x, float y, float scale, struct rm_vec3_t color);

/**
  @brief Calculates the width of the given text string when rendered
*/
enum reh_error_code_e rtr_CalculateTextWidth(const char *text, struct rtr_character_t *characters, float scale, float *totalWidth);

/**
  @brief Calculates the height of the given text string when rendered
*/
enum reh_error_code_e rtr_CalculateTextHeight(const char *text, struct rtr_character_t *characters, float scale, float *totalHeight, float *ascent);

/**
  @brief Formats a marker value into a string buffer
*/
enum reh_error_code_e rtr_FormatMarkerValue(float value, char* buffer, const int bufferSize);

/**
  @brief Converts NDC X coordinate to pixel X coordinate
*/
float rtr_NdcToPixelX(float ndcX);

/**
  @brief Converts NDC Y coordinate to pixel Y coordinate
*/
float rtr_NdcToPixelY(float ndcY);

/**
  @brief Converts world X coordinate to pixel X coordinate
*/
float rtr_WorldToPixelX(float worldX);

/**
  @brief Converts world Y coordinate to pixel Y coordinate
*/
float rtr_WorldToPixelY(float worldY);

/**
  @brief Renders axis labels
*/
enum reh_error_code_e rtr_RenderAxisLabels(GLuint program, GLuint VAO, GLuint VBO, struct rtr_character_t *characters, float scale, struct rm_vec3_t color);
#endif // TEXT_H
