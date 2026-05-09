#include "libobjc3c_frontend/frontend_compile_input.h"

#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "support/objc3_file_reading.h"

namespace objc3c::frontend {

namespace {

const char *EntrypointName(FrontendCompileEntrypointKind kind) {
  switch (kind) {
    case FrontendCompileEntrypointKind::kFile:
      return "compile_file";
    case FrontendCompileEntrypointKind::kSource:
      return "compile_source";
  }
  return "compile";
}

bool ValidateKindSpecificOptions(FrontendCompileEntrypointKind kind,
                                 const objc3c_frontend_compile_options_t &options,
                                 std::string &error) {
  switch (kind) {
    case FrontendCompileEntrypointKind::kFile:
      return ValidateFrontendCompileFileOptions(options, error);
    case FrontendCompileEntrypointKind::kSource:
      return ValidateFrontendCompileSourceOptions(options, error);
  }
  error = "unknown frontend compile entrypoint.";
  return false;
}

}  // namespace

objc3c_frontend_status_t ValidateFrontendCompileEntrypointArguments(
    FrontendCompileEntrypointKind kind,
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result,
    bool &accepted) {
  accepted = false;
  if (result == nullptr) {
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  if (context == nullptr) {
    return SetFrontendUsageErrorWithoutContext(
        result, std::string(EntrypointName(kind)) +
                    " requires a frontend context.");
  }
  if (options == nullptr) {
    return SetFrontendUsageError(context, result,
                                 std::string(EntrypointName(kind)) +
                                     " requires compile options.");
  }
  accepted = true;
  return OBJC3C_FRONTEND_STATUS_OK;
}

objc3c_frontend_status_t ValidateFrontendCompileEntrypointOptions(
    FrontendCompileEntrypointKind kind,
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t &options,
    objc3c_frontend_compile_result_t *result,
    bool &accepted) {
  accepted = false;

  std::string language_version_error;
  if (!ValidateSupportedFrontendLanguageVersion(options.language_version,
                                                language_version_error)) {
    return SetFrontendUsageError(context, result, language_version_error);
  }

  std::string compile_options_error;
  if (!ValidateKindSpecificOptions(kind, options, compile_options_error)) {
    return SetFrontendUsageError(context, result, compile_options_error);
  }

  accepted = true;
  return OBJC3C_FRONTEND_STATUS_OK;
}

bool LoadFrontendCompileFileInput(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t &options,
    objc3c_frontend_compile_result_t *result,
    FrontendCompileInput &input) {
  input.input_path = BorrowedFrontendPathToFilesystemPath(options.input_path);
  std::string io_error;
  if (objc3c::support::TryReadTextFile(
          input.input_path, input.source_text, io_error,
          "failed to open input source '" + input.input_path.string() + "'",
          "failed while reading input source '" + input.input_path.string() +
              "'")) {
    return true;
  }

  SetFrontendUsageError(context, result, io_error);
  return false;
}

FrontendCompileInput BuildFrontendCompileSourceInput(
    const objc3c_frontend_compile_options_t &options) {
  FrontendCompileInput input;
  input.input_path = ResolveFrontendInputPath(options);
  input.source_text = std::string(options.source_text);
  return input;
}

}  // namespace objc3c::frontend
