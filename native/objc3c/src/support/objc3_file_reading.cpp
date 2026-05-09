#include "support/objc3_file_reading.h"

#include <fstream>
#include <sstream>

namespace objc3c::support {

bool TryReadTextFile(const std::filesystem::path &path,
                     std::string &contents,
                     std::string &error,
                     const std::string &open_error,
                     const std::string &read_error) {
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    error = open_error;
    return false;
  }
  std::ostringstream buffer;
  buffer << input.rdbuf();
  if (!input.good() && !input.eof()) {
    error = read_error;
    return false;
  }
  contents = buffer.str();
  return true;
}

}  // namespace objc3c::support
