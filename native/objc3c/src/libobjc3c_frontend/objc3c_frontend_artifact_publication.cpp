#include "libobjc3c_frontend/objc3c_frontend_artifact_publication.h"

#include "libobjc3c_frontend/objc3c_frontend_file_output.h"

namespace objc3c::frontend {

bool WriteFrontendTextArtifactOrError(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::filesystem::path &path,
    const std::string &contents) {
  std::string io_error;
  if (WriteFrontendTextFile(path, contents, io_error)) {
    return true;
  }
  SetFrontendPublicationError(context, result, io_error);
  return false;
}

bool WriteFrontendBinaryArtifactOrError(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::filesystem::path &path,
    const std::string &contents) {
  std::string io_error;
  if (WriteFrontendBinaryFile(path, contents, io_error)) {
    return true;
  }
  SetFrontendPublicationError(context, result, io_error);
  return false;
}

bool WriteFrontendBinaryArtifactOrError(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::filesystem::path &path,
    const std::vector<uint8_t> &contents) {
  std::string io_error;
  if (WriteFrontendBinaryFile(path, contents, io_error)) {
    return true;
  }
  SetFrontendPublicationError(context, result, io_error);
  return false;
}

}  // namespace objc3c::frontend
