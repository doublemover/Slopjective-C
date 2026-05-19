#pragma once

#include <filesystem>
#include <string>

namespace objc3::io {

void EnsureOutputParentDirectory(const std::filesystem::path &path);
void WriteTextPayload(const std::filesystem::path &path,
                      const std::string &contents);
void WriteBinaryPayload(const std::filesystem::path &path,
                        const std::string &contents);
[[nodiscard]] std::string ReadTextPayload(const std::filesystem::path &path);

}  // namespace objc3::io
