#include "io/objc3_file_io.h"

#include <string>
#include <vector>

#include "io/objc3_file_payload_io.h"
#include "io/objc3_line_join.h"

void WriteText(const std::filesystem::path &path, const std::string &contents) {
  objc3::io::WriteTextPayload(path, contents);
}

void WriteBytes(const std::filesystem::path &path, const std::string &contents) {
  objc3::io::WriteBinaryPayload(path, contents);
}

std::string ReadText(const std::filesystem::path &path) {
  return objc3::io::ReadTextPayload(path);
}

std::string JoinLines(const std::vector<std::string> &lines) {
  return objc3::io::JoinLinesWithTrailingNewline(lines);
}
