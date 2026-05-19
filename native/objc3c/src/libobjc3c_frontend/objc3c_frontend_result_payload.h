#pragma once

/*
 * Internal C++ owner for populating result-owned public payload strings.
 * Low-level string allocation/release remains in objc3c_frontend_result_ownership.h.
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

bool PopulateCompileResultOwnedPayload(
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendResultOwnedPayload &payload,
    std::string &error);

bool SetCompileResultOwnedErrorMessage(
    objc3c_frontend_compile_result_t *result,
    const std::string &message,
    std::string &error);

}  // namespace objc3c::frontend
