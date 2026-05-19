#pragma once

#include "libobjc3c_frontend/c_api_types.h"

struct objc3c_frontend_c_result {
  objc3c_frontend_c_compile_result_t storage;
};

namespace objc3c::frontend::c_api {

objc3c_frontend_c_compile_result_t *MutableOwnedResultStorage(
    objc3c_frontend_c_result_t *result);

const objc3c_frontend_c_compile_result_t *BorrowOwnedResultStorage(
    const objc3c_frontend_c_result_t *result);

}  // namespace objc3c::frontend::c_api
