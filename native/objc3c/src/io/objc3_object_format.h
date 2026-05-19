#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

enum class ProducedObjectFormat : std::uint8_t {
  kUnknown = 0,
  kCoff = 1,
  kElf = 2,
  kMachO = 3,
};

ProducedObjectFormat DetectProducedObjectFormat(
    const std::filesystem::path &object_out);
std::string ProducedObjectFormatName(ProducedObjectFormat format);
void NormalizeObjectDeterminism(const std::filesystem::path &object_out);
