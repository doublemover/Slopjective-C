#pragma once

/*
 * Internal C++ storage behind the opaque public frontend context. Public code
 * owns only objc3c_frontend_context_t handles and error-copy semantics.
 */
#include <cstddef>
#include <mutex>
#include <string>

#include "libobjc3c_frontend/objc3c_frontend_context.h"
#include "libobjc3c_frontend/objc3c_frontend_result.h"

struct objc3c_frontend_context {
  mutable std::mutex mutex;
  std::string last_error;
  std::string diagnostics_path;
  std::string manifest_path;
  std::string runtime_metadata_binary_path;
  std::string ir_path;
  std::string object_path;
};

namespace objc3c::frontend {

std::mutex &FrontendContextMutex(objc3c_frontend_context_t *context);

void SetFrontendContextError(objc3c_frontend_context_t *context,
                             const char *message);

void ClearFrontendContextResultPaths(objc3c_frontend_context_t *context);

void SetFrontendContextDiagnosticsPath(objc3c_frontend_context_t *context,
                                       const std::string &path);

void SetFrontendContextManifestPath(objc3c_frontend_context_t *context,
                                    const std::string &path);

void SetFrontendContextRuntimeMetadataPath(objc3c_frontend_context_t *context,
                                           const std::string &path);

void SetFrontendContextIrPath(objc3c_frontend_context_t *context,
                              const std::string &path);

void SetFrontendContextObjectPath(objc3c_frontend_context_t *context,
                                  const std::string &path);

bool PopulateCompileResultFromFrontendContext(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    std::string &error);

objc3c_frontend_status_t ReturnWithFrontendContextResultPayload(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result);

objc3c_frontend_status_t SetFrontendUsageError(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::string &message);

objc3c_frontend_status_t SetFrontendUsageErrorWithoutContext(
    objc3c_frontend_compile_result_t *result,
    const std::string &message);

size_t CopyFrontendContextLastError(const objc3c_frontend_context_t *context,
                                    char *buffer,
                                    size_t buffer_size);

}  // namespace objc3c::frontend
