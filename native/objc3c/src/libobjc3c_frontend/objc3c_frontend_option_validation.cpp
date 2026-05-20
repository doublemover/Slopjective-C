#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"

#include <cstddef>
#include <cstdint>

namespace objc3c::frontend {

bool ValidateFrontendEmitOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error) {
  if (options.allow_live_error_runtime_surface > 1u) {
    error =
        "compile_options.allow_live_error_runtime_surface must be 0 or 1.";
    return false;
  }
  if (options.imported_runtime_surface_path_count > 0 &&
      options.imported_runtime_surface_paths == nullptr) {
    error =
        "compile_options.imported_runtime_surface_paths is required when imported_runtime_surface_path_count is non-zero.";
    return false;
  }
  for (size_t index = 0; index < options.imported_runtime_surface_path_count;
       ++index) {
    if (IsMissingFrontendBorrowedPath(
            options.imported_runtime_surface_paths[index])) {
      error =
          "compile_options.imported_runtime_surface_paths cannot contain empty paths.";
      return false;
    }
  }
  const bool wants_artifact = options.emit_manifest != 0 ||
                              options.emit_ir != 0 ||
                              options.emit_object != 0;
  if (wants_artifact && IsMissingFrontendBorrowedPath(options.out_dir)) {
    error =
        "emit_manifest/emit_ir/emit_object require compile_options.out_dir.";
    return false;
  }
  if (wants_artifact && IsMissingFrontendBorrowedText(options.emit_prefix)) {
    error =
        "emit_manifest/emit_ir/emit_object require compile_options.emit_prefix.";
    return false;
  }

  if (options.emit_object == 0) {
    return true;
  }

  const bool wants_clang_backend =
      options.ir_object_backend ==
      static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG);
  const bool wants_llvm_direct_backend =
      options.ir_object_backend ==
      static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT);
  if (!wants_clang_backend && !wants_llvm_direct_backend) {
    error = "emit_object requires compile_options.ir_object_backend to be clang or llvm-direct.";
    return false;
  }
  if (wants_clang_backend && IsMissingFrontendBorrowedPath(options.clang_path)) {
    error = "emit_object requires compile_options.clang_path for clang backend.";
    return false;
  }
  if (wants_llvm_direct_backend &&
      IsMissingFrontendBorrowedPath(options.llc_path)) {
    error = "emit_object requires compile_options.llc_path for llvm-direct backend.";
    return false;
  }
  return true;
}

bool ValidateFrontendCompileFileOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error) {
  if (IsMissingFrontendBorrowedPath(options.input_path)) {
    error = "compile_file requires compile_options.input_path.";
    return false;
  }
  return ValidateFrontendEmitOptions(options, error);
}

bool ValidateFrontendCompileSourceOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error) {
  if (IsMissingFrontendBorrowedText(options.source_text)) {
    error = "compile_source requires compile_options.source_text.";
    return false;
  }
  return ValidateFrontendEmitOptions(options, error);
}

}  // namespace objc3c::frontend
