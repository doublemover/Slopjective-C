#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"

namespace objc3c::frontend {

bool IsMissingFrontendBorrowedText(objc3c_frontend_borrowed_text_t text) {
  return text == nullptr || text[0] == '\0';
}

bool IsMissingFrontendBorrowedPath(objc3c_frontend_borrowed_path_t path) {
  return path == nullptr || path[0] == '\0';
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

}  // namespace objc3c::frontend
