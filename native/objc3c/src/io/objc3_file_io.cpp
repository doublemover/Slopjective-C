#include "io/objc3_file_io.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void WriteText(const std::filesystem::path &path, const std::string &contents) {
  const std::filesystem::path parent = path.parent_path();
  if (!parent.empty()) {
    std::filesystem::create_directories(parent);
  }
  std::ofstream out(path, std::ios::binary);
  out << contents;
}

std::string ReadText(const std::filesystem::path &path) {
  std::ifstream input(path, std::ios::binary);
  std::ostringstream buffer;
  buffer << input.rdbuf();
  return buffer.str();
}

std::string JoinLines(const std::vector<std::string> &lines) {
  std::ostringstream out;
  for (const auto &line : lines) {
    out << line << "\n";
  }
  return out.str();
}
