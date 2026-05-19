#include "io/objc3_file_payload_io.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace objc3::io {

void EnsureOutputParentDirectory(const std::filesystem::path &path) {
  const std::filesystem::path parent = path.parent_path();
  if (parent.empty()) {
    return;
  }

  std::error_code mkdir_error;
  std::filesystem::create_directories(parent, mkdir_error);
  if (mkdir_error) {
    throw std::runtime_error("failed to create output directory '" +
                             parent.string() + "': " +
                             mkdir_error.message());
  }
}

void WriteTextPayload(const std::filesystem::path &path,
                      const std::string &contents) {
  EnsureOutputParentDirectory(path);
  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) {
    throw std::runtime_error("failed to open output file '" + path.string() +
                             "' for writing");
  }
  out << contents;
  if (!out.good()) {
    throw std::runtime_error("failed while writing output file '" +
                             path.string() + "'");
  }
}

void WriteBinaryPayload(const std::filesystem::path &path,
                        const std::string &contents) {
  EnsureOutputParentDirectory(path);
  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) {
    throw std::runtime_error("failed to open output file '" + path.string() +
                             "' for writing");
  }
  out.write(contents.data(), static_cast<std::streamsize>(contents.size()));
  if (!out.good()) {
    throw std::runtime_error("failed while writing output file '" +
                             path.string() + "'");
  }
}

std::string ReadTextPayload(const std::filesystem::path &path) {
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    throw std::runtime_error("failed to open input file '" + path.string() +
                             "'");
  }
  std::ostringstream buffer;
  buffer << input.rdbuf();
  if (!input.good() && !input.eof()) {
    throw std::runtime_error("failed while reading input file '" +
                             path.string() + "'");
  }
  return buffer.str();
}

}  // namespace objc3::io
