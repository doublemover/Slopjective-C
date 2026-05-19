#include "libobjc3c_frontend/objc3c_frontend_file_output.h"

#include <fstream>
#include <limits>
#include <system_error>

namespace objc3c::frontend {

namespace {

bool PrepareFrontendOutputDirectory(const std::filesystem::path &path,
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
  return true;
}

bool WriteFrontendBytes(const std::filesystem::path &path,
                        const char *contents,
                        std::size_t size,
                        std::string &error) {
  if (!PrepareFrontendOutputDirectory(path, error)) {
    return false;
  }
  if (size >
      static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max())) {
    error = "output file '" + path.string() +
            "' is too large to write in one operation";
    return false;
  }
  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) {
    error = "failed to open output file '" + path.string() + "' for writing";
    return false;
  }
  if (size > 0u) {
    out.write(contents, static_cast<std::streamsize>(size));
  }
  if (!out.good()) {
    error = "failed while writing output file '" + path.string() + "'";
    return false;
  }
  return true;
}

}  // namespace

bool WriteFrontendTextFile(const std::filesystem::path &path,
                           const std::string &contents,
                           std::string &error) {
  return WriteFrontendBytes(path, contents.data(), contents.size(), error);
}

bool WriteFrontendBinaryFile(const std::filesystem::path &path,
                             const std::string &contents,
                             std::string &error) {
  return WriteFrontendBytes(path, contents.data(), contents.size(), error);
}

bool WriteFrontendBinaryFile(const std::filesystem::path &path,
                             std::span<const std::uint8_t> contents,
                             std::string &error) {
  return WriteFrontendBytes(
      path, reinterpret_cast<const char *>(contents.data()), contents.size(),
      error);
}

bool WriteFrontendBinaryFile(const std::filesystem::path &path,
                             const std::vector<std::uint8_t> &contents,
                             std::string &error) {
  return WriteFrontendBinaryFile(
      path, std::span<const std::uint8_t>(contents.data(), contents.size()),
      error);
}

}  // namespace objc3c::frontend
