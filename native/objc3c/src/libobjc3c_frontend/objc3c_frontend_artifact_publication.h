#pragma once

/*
 * Internal C++ artifact publication owner. This maps artifact write outcomes
 * into public result/context payloads without defining public ABI layout.
 */
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "libobjc3c_frontend/objc3c_frontend_context.h"
#include "libobjc3c_frontend/objc3c_frontend_result.h"

namespace objc3c::frontend {

void SetFrontendPublicationError(objc3c_frontend_context_t *context,
                                 objc3c_frontend_compile_result_t *result,
                                 const std::string &message);

void SetFrontendDiagnosticOrOkStatus(
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product);

bool WriteFrontendTextArtifactOrError(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::filesystem::path &path,
    const std::string &contents);

bool WriteFrontendBinaryArtifactOrError(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::filesystem::path &path,
    const std::string &contents);

bool WriteFrontendBinaryArtifactOrError(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::filesystem::path &path,
    const std::vector<uint8_t> &contents);

}  // namespace objc3c::frontend
