#pragma once

#include <filesystem>
#include <string>

namespace objc3c::support {

bool TryReadTextFile(const std::filesystem::path &path,
                     std::string &contents,
                     std::string &error,
                     const std::string &open_error,
                     const std::string &read_error);

}  // namespace objc3c::support
