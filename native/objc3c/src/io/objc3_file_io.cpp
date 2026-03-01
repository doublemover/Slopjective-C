#include "io/objc3_file_io.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

void WriteText(const std::filesystem::path &path, const std::string &contents) {
  const std::filesystem::path parent = path.parent_path();
  if (!parent.empty()) {
    std::error_code mkdir_error;
    std::filesystem::create_directories(parent, mkdir_error);
    if (mkdir_error) {
      throw std::runtime_error("failed to create output directory '" + parent.string() + "': " + mkdir_error.message());
    }
  }
  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) {
    throw std::runtime_error("failed to open output file '" + path.string() + "' for writing");
  }
  out << contents;
  if (!out.good()) {
    throw std::runtime_error("failed while writing output file '" + path.string() + "'");
  }
}

std::string ReadText(const std::filesystem::path &path) {
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    throw std::runtime_error("failed to open input file '" + path.string() + "'");
  }
  std::ostringstream buffer;
  buffer << input.rdbuf();
  if (!input.good() && !input.eof()) {
    throw std::runtime_error("failed while reading input file '" + path.string() + "'");
  }
  return buffer.str();
}

std::string JoinLines(const std::vector<std::string> &lines) {
  std::ostringstream out;
  for (const auto &line : lines) {
    out << line << "\n";
  }
  return out.str();
}
