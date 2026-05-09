#pragma once

#include <filesystem>
#include <string>

namespace objc3c::frontend {

bool WriteFrontendTextFile(const std::filesystem::path &path,
                           const std::string &contents,
                           std::string &error);

bool WriteFrontendBinaryFile(const std::filesystem::path &path,
                             const std::string &contents,
                             std::string &error);

}  // namespace objc3c::frontend
