#include "libobjc3c_frontend/objc3c_frontend_file_output.h"

#include <fstream>
#include <system_error>

namespace objc3c::frontend {

bool WriteFrontendTextFile(const std::filesystem::path &path,
                           const std::string &contents,
                           std::string &error) {
  std::error_code mkdir_error;
  if (!path.parent_path().empty()) {
    std::filesystem::create_directories(path.parent_path(), mkdir_error);
  }
  if (mkdir_error) {
    error = "failed to create output directory '" +
            path.parent_path().string() + "': " + mkdir_error.message();
    return false;
  }

  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) {
    error = "failed to open output file '" + path.string() + "' for writing";
    return false;
  }
  out << contents;
  if (!out.good()) {
    error = "failed while writing output file '" + path.string() + "'";
    return false;
  }
  return true;
}

bool WriteFrontendBinaryFile(const std::filesystem::path &path,
                             const std::string &contents,
                             std::string &error) {
  std::error_code mkdir_error;
  if (!path.parent_path().empty()) {
    std::filesystem::create_directories(path.parent_path(), mkdir_error);
  }
  if (mkdir_error) {
    error = "failed to create output directory '" +
            path.parent_path().string() + "': " + mkdir_error.message();
    return false;
  }

  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) {
    error = "failed to open output file '" + path.string() + "' for writing";
    return false;
  }
  out.write(contents.data(), static_cast<std::streamsize>(contents.size()));
  if (!out.good()) {
    error = "failed while writing output file '" + path.string() + "'";
    return false;
  }
  return true;
}

}  // namespace objc3c::frontend
