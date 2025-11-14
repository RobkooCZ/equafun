#include "core/errorHandler.h"
#include "math/typeConvert.h"

enum reh_error_code_e rm_Mat4ValuePtr(struct rm_mat4_t *mat, float **result){
  if (mat == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Matrix pointer in rm_Mat4ValuePtr is NULL");
  }

  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Mat4ValuePtr is NULL");
  }

  *result = mat->data;

  return ERR_SUCCESS;
}

