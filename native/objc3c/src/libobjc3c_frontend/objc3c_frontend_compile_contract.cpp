#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"

#include <cstdint>

namespace objc3c::frontend {

namespace {

constexpr const char *kDefaultEmitPrefix = "module";
constexpr const char *kDefaultMemoryInputPath = "<memory>";

}  // namespace

bool IsMissingFrontendBorrowedText(objc3c_frontend_borrowed_text_t text) {
  return text == nullptr || text[0] == '\0';
}

bool IsMissingFrontendBorrowedPath(objc3c_frontend_borrowed_path_t path) {
  return path == nullptr || path[0] == '\0';
}

bool ValidateFrontendEmitOptions(
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

uint8_t NormalizeFrontendLanguageVersion(uint8_t requested_language_version) {
  if (requested_language_version == 0u) {
    return static_cast<uint8_t>(OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT);
  }
  return requested_language_version;
}

bool ValidateSupportedFrontendLanguageVersion(uint8_t requested_language_version,
                                              std::string &error) {
  const uint8_t normalized_language_version =
      NormalizeFrontendLanguageVersion(requested_language_version);
  if (normalized_language_version ==
      static_cast<uint8_t>(OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3)) {
    return true;
  }

  error = "unsupported compile_options.language_version: " +
          std::to_string(normalized_language_version) +
          " (only Objective-C version 3 is supported).";
  return false;
}

std::filesystem::path BorrowedFrontendPathToFilesystemPath(
    objc3c_frontend_borrowed_path_t path) {
  return std::filesystem::path(path);
}

std::filesystem::path OptionalBorrowedFrontendFilesystemPath(
    objc3c_frontend_borrowed_path_t path) {
  if (IsMissingFrontendBorrowedPath(path)) {
    return std::filesystem::path();
  }
  return BorrowedFrontendPathToFilesystemPath(path);
}

std::filesystem::path ResolveFrontendInputPath(
    const objc3c_frontend_compile_options_t &options) {
  if (!IsMissingFrontendBorrowedPath(options.input_path)) {
    return BorrowedFrontendPathToFilesystemPath(options.input_path);
  }
  return std::filesystem::path(kDefaultMemoryInputPath);
}

std::filesystem::path ResolveFrontendOutputDir(
    const objc3c_frontend_compile_options_t &options) {
  if (IsMissingFrontendBorrowedPath(options.out_dir)) {
    return std::filesystem::path();
  }
  return BorrowedFrontendPathToFilesystemPath(options.out_dir);
}

std::string ResolveFrontendEmitPrefix(
    const objc3c_frontend_compile_options_t &options,
    const std::filesystem::path &input_path) {
  if (!IsMissingFrontendBorrowedText(options.emit_prefix)) {
    return std::string(options.emit_prefix);
  }
  const std::string stem = input_path.stem().string();
  if (!stem.empty()) {
    return stem;
  }
  return kDefaultEmitPrefix;
}

Objc3FrontendOptions BuildFrontendPipelineOptions(
    const objc3c_frontend_compile_options_t &options) {
  Objc3FrontendOptions frontend_options;
  frontend_options.language_version =
      NormalizeFrontendLanguageVersion(options.language_version);
  frontend_options.language_profile = Objc3FrontendLanguageProfile::kCanonical;
  frontend_options.emit_manifest = options.emit_manifest != 0;
  frontend_options.emit_ir = options.emit_ir != 0;
  frontend_options.emit_object = options.emit_object != 0;
  if (options.translation_unit_registration_order_ordinal > 0) {
    frontend_options.bootstrap_registration_order_ordinal =
        options.translation_unit_registration_order_ordinal;
  }
  if (options.max_message_send_args > 0) {
    frontend_options.lowering.max_message_send_args =
        options.max_message_send_args;
  }
  if (!IsMissingFrontendBorrowedText(options.runtime_dispatch_symbol)) {
    frontend_options.lowering.runtime_dispatch_symbol =
        options.runtime_dispatch_symbol;
  }
  return frontend_options;
}

}  // namespace objc3c::frontend
