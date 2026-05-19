#pragma once

/*
 * Internal C++ file write helpers for frontend artifact publication. These do
 * not expose package-facing file APIs.
 */
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace objc3c::frontend {

bool WriteFrontendTextFile(const std::filesystem::path &path,
                           const std::string &contents,
                           std::string &error);

bool WriteFrontendBinaryFile(const std::filesystem::path &path,
                             const std::string &contents,
                             std::string &error);

bool WriteFrontendBinaryFile(const std::filesystem::path &path,
                             std::span<const std::uint8_t> contents,
                             std::string &error);

bool WriteFrontendBinaryFile(const std::filesystem::path &path,
                             const std::vector<std::uint8_t> &contents,
                             std::string &error);

}  // namespace objc3c::frontend
