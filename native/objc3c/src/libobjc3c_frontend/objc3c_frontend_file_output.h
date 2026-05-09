#pragma once

/*
 * Internal C++ file write helpers for frontend artifact publication. These do
 * not expose package-facing file APIs.
 */
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
