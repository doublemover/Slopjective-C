#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace objc3::fuzz {
inline constexpr std::size_t kMaxFrontendInput = 65536;

struct FrontendObservation {
  int disposition = 0;
  std::size_t diagnostics = 0;
  std::string digest;
};

FrontendObservation Frontend(const std::uint8_t *data, std::size_t size);
std::vector<std::uint8_t> FrontendSeed(unsigned variant);
} // namespace objc3::fuzz
