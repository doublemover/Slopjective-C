#pragma once

/*
 * Internal C++ owner for allocation, release, and population of public result
 * strings. Callers use objc3c_frontend_result.h or c_api.h destruction APIs.
 */
#include <string>

#include "libobjc3c_frontend/objc3c_frontend_result.h"

namespace objc3c::frontend {

struct Objc3FrontendResultOwnedPayload {
  std::string error_message;
  std::string diagnostics_path;
  std::string manifest_path;
  std::string runtime_metadata_path;
  std::string ir_path;
  std::string object_path;
};

void ResetCompileResultForWrite(objc3c_frontend_compile_result_t *result);

objc3c_frontend_string_t *CloneOwnedFrontendString(const std::string &text);

void ReleaseOwnedFrontendString(objc3c_frontend_string_t *string);

void ReleaseCompileResultOwnedStrings(objc3c_frontend_compile_result_t *result);

bool PopulateCompileResultOwnedPayload(
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendResultOwnedPayload &payload,
    std::string &error);

bool SetCompileResultOwnedErrorMessage(
    objc3c_frontend_compile_result_t *result,
    const std::string &message,
    std::string &error);

}  // namespace objc3c::frontend
