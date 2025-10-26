#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include "textRenderer/text.h"
#include "core/window.h"
#include "renderer/graph.h"
#include "utils/shaderUtils.h"
#include "core/errorHandler.h"
#include "math/Vec2.h"

// static helper to be used internally
static const char* ft_ErrCodeToStr(FT_Error errCode){
  const char* errString = FT_Error_String(errCode);
  return errString ? errString : "Unknown error.";
}

enum reh_error_code_e rtr_initFt(FT_Library *library){
  FT_Error ftErr = FT_Init_FreeType(library);

  if (ftErr != FT_Err_Ok){
    SET_ERROR_RETURN(ERR_FT_FAILED_TO_INIT, "Failed to initialize FT: %s", ft_ErrCodeToStr(ftErr));
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e rtr_initFtFace(FT_Library *library, FT_Face *face){
  // FACE INITIALIZATION

  FT_Error faceInitErr = FT_New_Face(*library, "data/fonts/DejaVuSans.ttf", 0, face);

  if (faceInitErr == FT_Err_Unknown_File_Format){
    SET_ERROR_RETURN(ERR_FT_FACE_UNKNOWN_FILE_FORMAT, "Failed to initialize FT face, unsupported font format.");
  }
  else if (faceInitErr != FT_Err_Ok){
    SET_ERROR_RETURN(ERR_FT_FAILED_TO_INIT, "Failed to initialize FT face: %s", ft_ErrCodeToStr(faceInitErr));
  }

  // SETTING FACE INFO
  FT_Error setSizeErr = FT_Set_Pixel_Sizes(*face, 0, 16);

  if (setSizeErr != FT_Err_Ok){
    SET_ERROR_RETURN(ERR_FT_FACE_FAILED_TO_SET_FONT_SIZE, "Failed to set font size: %s", ft_ErrCodeToStr(setSizeErr));
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e rtr_loadChar(FT_Face face, uint8_t ch){
  FT_Error charErr = FT_Load_Char(face, ch, FT_LOAD_RENDER); // FT_LOAD_RENDER flag tells FT to create an 8-bit grayscale bitmap image for us accessable via face->glyph->bitmap
  if (charErr != FT_Err_Ok){
    SET_ERROR_RETURN(ERR_FT_FAILED_TO_LOAD_CHAR, "Failed to load char: %s", ft_ErrCodeToStr(charErr));
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e rtr_loadCharactersIntoArray(FT_Face face, struct rtr_character_t *characters){
  glad_glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

  // load the first ASCII_CHAR_COUNT (rn 128) characters into the provided array of structs
  for (struct rtr_character_t *ptr = characters; ptr < characters + ASCII_CHAR_COUNT; ++ptr){
    // load char glyph
    CHECK_ERROR_CTX(rtr_loadChar(face, ptr - characters), "Failed to load character.");

    // generate a texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // store character for later use
    ptr->textureID = texture;
    ptr->size = (struct rm_vec2_t){face->glyph->bitmap.width, face->glyph->bitmap.rows};
    ptr->bearing = (struct rm_vec2_t){face->glyph->bitmap_left, face->glyph->bitmap_top};
    ptr->advance = face->glyph->advance.x;
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e rtr_createTextRenderVAO(GLuint *VAO, GLuint *VBO){
  if (VAO == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "VAO pointer is NULL in rtr_createTextRenderVAO()");
  }
  if (VBO == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "VBO pointer is NULL in rtr_createTextRenderVAO()");
  }

  glGenVertexArrays(1, VAO);
  glGenBuffers(1, VBO);

  glBindVertexArray(*VAO);
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return ERR_SUCCESS;
}

enum reh_error_code_e rtr_renderText(GLuint program, GLuint VAO, GLuint VBO, const char *text, struct rtr_character_t *characters, float x, float y, float scale, struct rm_vec3_t color){
  if (text == nullptr || characters == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Text or characters pointer is NULL in rtr_renderText()");
  }

  if (program == 0 || VAO == 0 || VBO == 0){
    SET_ERROR_RETURN(ERR_RENDER_INVALID_PARAMS, "Invalid program, VAO or VBO in rtr_renderText()");
  }

  // use our shader
  glUseProgram(program);

  // set the text to the provided color
  gluSet3f(program, "textColor", color.x, color.y, color.z);

  // activate corresponding render state
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  // iterate through all the characters and draw them
  for (const char *c = text; *c != '\0'; c++){
    struct rtr_character_t ch = characters[(uint8_t)*c]; // get the character glyph

    float xpos = x + ch.bearing.x * scale;
    float ypos = y - (ch.size.y - ch.bearing.y) * scale;

    float w = ch.size.x * scale;
    float h = ch.size.y * scale;

    float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },

        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }
    };

    glBindTexture(GL_TEXTURE_2D, ch.textureID);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Advance cursor (FT gives advance in 1/64th pixels)
    x += (ch.advance >> 6) * scale;
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  return ERR_SUCCESS;
}

enum reh_error_code_e rtr_calculateTextWidth(const char *text, struct rtr_character_t *characters, float scale, float *totalWidth){
  if (text == nullptr || characters == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Text or characters pointer is NULL in rtr_calculateTextWidth()");
  }
  if (totalWidth == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Total width pointer is NULL in rtr_calculateTextWidth()");
  }

  *totalWidth = 0;

  for (const char *ptr = text; *ptr != '\0'; ++ptr){
    int advance = characters[ptr - text].advance;

    *totalWidth += (advance >> 6) * scale;
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e rtr_calculateTextHeight(const char *text, struct rtr_character_t *characters, float scale, float *totalHeight, float *ascent){
  if (text == nullptr || characters == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Text or characters pointer is NULL in rtr_calculateTextHeight()");
  }
  if (totalHeight == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Total height pointer is NULL in rtr_calculateTextHeight()");
  }
  if (ascent == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Ascent pointer is NULL in rtr_calculateTextHeight()");
  }

  float maxAscent = 0.0f;
  float maxDescent = 0.0f;

  for (const char *ptr = text; *ptr != '\0'; ++ptr){
    struct rtr_character_t ch = characters[(unsigned char)*ptr];

    float ascent = (float)ch.bearing.y * scale;
    float descent = ((float)(ch.size.y - ch.bearing.y)) * scale;

    if (ascent > maxAscent) maxAscent = ascent;
    if (descent > maxDescent) maxDescent = descent;
  }

  *totalHeight = maxAscent + maxDescent;
  *ascent = maxAscent;

  return ERR_SUCCESS;
}

enum reh_error_code_e rtr_formatMarkerValue(float value, char *buffer, const int bufferSize){
  if (value < -FLT_MAX){
    SET_ERROR_TECHNICAL_RETURN(ERR_UNDERFLOW, "value < -FLT_MAX (%f)", "Value provided to rtr_formatMarkerValue() is smaller than -FLT_MAX", -FLT_MAX);
  }
  if (value > FLT_MAX){
    SET_ERROR_TECHNICAL_RETURN(ERR_OVERFLOW, "value > FLT_MAX (%f)", "Value provided to rtr_formatMarkerValue() is bigger than FLT_MAX (%f)", FLT_MAX);
  }
  if (buffer == nullptr){
    SET_ERROR_TECHNICAL_RETURN(ERR_INVALID_POINTER, "buffer == nullptr", "Buffer provided to rtr_formatMarkerValue() is NULL");
  }
  if (bufferSize <= 0){
    SET_ERROR_TECHNICAL_RETURN(ERR_INVALID_INPUT, "bufferSize <= 0 (%d)", "Buffer size provided to rtr_formatMarkerValue is 0 or less. (%d)", bufferSize);
  }

  // Check if value is effectively an integer (e.g., 1.00, -5.00)
  if (fmodf(value, 1.0f) == 0.0f){
    // It's an integer, format without decimals
    snprintf(buffer, bufferSize, "%d", (int)value);
  } else {
    // Has decimal places, format with 2 decimals
    snprintf(buffer, bufferSize, "%.2f", value);
  }

  return ERR_SUCCESS;
}

float rtr_ndcToPixelX(float ndcX){
  return ((ndcX + 1.0f) / 2.0f) * WIDTH;
}

float rtr_ndcToPixelY(float ndcY){
  return ((ndcY + 1.0f) / 2.0f) * HEIGHT;
}

enum reh_error_code_e rtr_renderAxisLabels(GLuint program, GLuint VAO, GLuint VBO, struct rtr_character_t *characters, float scale, struct rm_vec3_t color){
  // [0,0] point
  int value = 0;
  char label[5];
  CHECK_ERROR_CTX(rtr_formatMarkerValue(value, label, 5), "Failed to format marker value."); // put the value into the string

  CHECK_ERROR_CTX(rtr_renderText(program, VAO, VBO, label, characters, rtr_ndcToPixelX(0.0f + POINT_MARKER_HEIGHT * 0.5f), rtr_ndcToPixelY(0.0f - POINT_MARKER_HEIGHT * 1.5f), scale, color), "Failed to render point [0,0]");

  // positive x axis labels
  int markerIndex = 0;
  for (float ndcX = GRID_SPACING_NDC; ndcX <= 1.0f; ndcX += GRID_SPACING_NDC){
    markerIndex++;

    // print the label into the buffer
    char label[5];
    CHECK_ERROR_CTX(rtr_formatMarkerValue((float)markerIndex, label, 5), "Failed to format marker value.");

    // get the text width so we can center the label below the marker
    float textWidth;
    CHECK_ERROR_CTX(rtr_calculateTextWidth(label, characters, scale, &textWidth), "Failed to calculate text width.");

    // where to render
    float pixelX = rtr_ndcToPixelX(ndcX);

    float labelX = pixelX - (textWidth / 2.0f);
    float labelY = rtr_ndcToPixelY(0.0f - (POINT_MARKER_HEIGHT * 3));

    // render
    CHECK_ERROR_CTX(rtr_renderText(program, VAO, VBO, label, characters, labelX, labelY, scale, color), "Failed to render text.");
  }

  // negative x axis labels
  markerIndex = 0;
  for (float ndcX = -GRID_SPACING_NDC; ndcX >= -1.0f; ndcX -= GRID_SPACING_NDC){
    markerIndex--;

    // print the label into the buffer
    char label[6];
    CHECK_ERROR_CTX(rtr_formatMarkerValue((float)markerIndex, label, 6), "Failed to format marker value.");

    // get the text width so we can center the label below the marker
    float textWidth;
    CHECK_ERROR_CTX(rtr_calculateTextWidth(label, characters, scale, &textWidth), "Failed to calculate text width.");

    // where to render
    float pixelX = rtr_ndcToPixelX(ndcX);

    float labelX = pixelX - (textWidth / 2.0f);
    float labelY = rtr_ndcToPixelY(0.0f - (POINT_MARKER_HEIGHT * 3));

    // render
    CHECK_ERROR_CTX(rtr_renderText(program, VAO, VBO, label, characters, labelX, labelY, scale, color), "Failed to render text.");
  }

  // positive y axis labels
  markerIndex = 0;
  for (float ndcY = GRID_SPACING_NDC; ndcY <= 1.0f; ndcY += GRID_SPACING_NDC){
    markerIndex++;

    // print the label into the buffer
    char label[5];
    CHECK_ERROR_CTX(rtr_formatMarkerValue((float)markerIndex, label, 5), "Failed to format marker value.");

    // get the text width so we can center the label below the marker
    float textHeight;
    float ascent;
    CHECK_ERROR_CTX(rtr_calculateTextHeight(label, characters, scale, &textHeight, &ascent), "Failed to calculate text height.");

    // where to render
    float pixelY = rtr_ndcToPixelY(ndcY);

    float labelX = rtr_ndcToPixelX(0.0f + (POINT_MARKER_HEIGHT * 1.5f));
    float labelY = pixelY + (textHeight / 2.0f) - ascent;

    // render
    CHECK_ERROR_CTX(rtr_renderText(program, VAO, VBO, label, characters, labelX, labelY, scale, color), "Failed to render text.");
  }

  // negative y axis labels
  markerIndex = 0;
  for (float ndcY = -GRID_SPACING_NDC; ndcY >= -1.0f; ndcY -= GRID_SPACING_NDC){
    markerIndex--;

    // print the label into the buffer
    char label[6];
    CHECK_ERROR_CTX(rtr_formatMarkerValue((float)markerIndex, label, 6), "Failed to format marker value.");

    // get the text width so we can center the label below the marker
    float textHeight;
    float ascent;
    CHECK_ERROR_CTX(rtr_calculateTextHeight(label, characters, scale, &textHeight, &ascent), "Failed to calculate text height.");

    // where to render
    float pixelY = rtr_ndcToPixelY(ndcY);

    float labelX = rtr_ndcToPixelX(0.0f + (POINT_MARKER_HEIGHT * 1.5f));
    float labelY = pixelY + (textHeight / 2.0f) - ascent;

    // render
    CHECK_ERROR_CTX(rtr_renderText(program, VAO, VBO, label, characters, labelX, labelY, scale, color), "Failed to render text.");
  }

  return ERR_SUCCESS;
}
