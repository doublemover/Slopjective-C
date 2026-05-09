#pragma once

#include <string>

#include "libobjc3c_frontend/objc3c_frontend_options.h"

namespace objc3c::frontend {

inline bool IsMissingFrontendBorrowedText(objc3c_frontend_borrowed_text_t text) {
  return text == nullptr || text[0] == '\0';
}

inline bool IsMissingFrontendBorrowedPath(objc3c_frontend_borrowed_path_t path) {
  return path == nullptr || path[0] == '\0';
}

inline bool ValidateFrontendEmitOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error) {
  if ((options.emit_ir != 0 || options.emit_object != 0) &&
      IsMissingFrontendBorrowedPath(options.out_dir)) {
    error = "emit_ir/emit_object require compile_options.out_dir.";
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
  if (wants_llvm_direct_backend && IsMissingFrontendBorrowedPath(options.llc_path)) {
    error = "emit_object requires compile_options.llc_path for llvm-direct backend.";
    return false;
  }
  return true;
}

inline bool ValidateFrontendCompileFileOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error) {
  if (IsMissingFrontendBorrowedPath(options.input_path)) {
    error = "compile_file requires compile_options.input_path.";
    return false;
  }
  return ValidateFrontendEmitOptions(options, error);
}

inline bool ValidateFrontendCompileSourceOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error) {
  if (IsMissingFrontendBorrowedText(options.source_text)) {
    error = "compile_source requires compile_options.source_text.";
    return false;
  }
  return ValidateFrontendEmitOptions(options, error);
}

}  // namespace objc3c::frontend
