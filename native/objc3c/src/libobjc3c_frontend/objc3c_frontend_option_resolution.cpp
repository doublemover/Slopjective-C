#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"

namespace objc3c::frontend {

namespace {

constexpr const char *kDefaultMemoryInputPath = "<memory>";

}  // namespace

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
  (void)input_path;
  return IsMissingFrontendBorrowedText(options.emit_prefix)
             ? std::string()
             : std::string(options.emit_prefix);
}

}  // namespace objc3c::frontend
