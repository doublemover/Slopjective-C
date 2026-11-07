#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace objc3::fuzz {
inline constexpr std::size_t kMaxPair = 131072;
inline constexpr std::size_t kMaxPart = 65536;
enum class Outcome { FramingRejected, SemanticRejected, Accepted };
Outcome Codec(const std::uint8_t *data, std::size_t size, std::string *observation = nullptr);
bool Unpack(const std::uint8_t *data, std::size_t size,
            std::string &source, std::string &json);
std::vector<std::uint8_t> Pack(const std::string &source, const std::string &json);
std::vector<std::uint8_t> Seed(unsigned variant);
} // namespace objc3::fuzz
