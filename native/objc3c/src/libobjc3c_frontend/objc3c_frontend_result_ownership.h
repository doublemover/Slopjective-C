#pragma once

/*
 * Internal C++ owner for allocation and release of public result strings.
 * Callers use objc3c_frontend_result.h or c_api.h destruction APIs.
 */
#include <string>

#include "libobjc3c_frontend/objc3c_frontend_result_types.h"

namespace objc3c::frontend {

void ResetCompileResultForWrite(objc3c_frontend_compile_result_t *result);

objc3c_frontend_string_t *CloneOwnedFrontendString(const std::string &text);

void ReleaseOwnedFrontendString(objc3c_frontend_string_t *string);

void ReleaseCompileResultOwnedStrings(objc3c_frontend_compile_result_t *result);

}  // namespace objc3c::frontend
