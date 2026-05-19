#pragma once

#include <cstdint>
#include <string>

inline std::uint64_t MixObjc3ParserContractFingerprint(
    const std::uint64_t fingerprint,
    const std::uint64_t value) {
  constexpr std::uint64_t kMixConstant = 1099511628211ull;
  return (fingerprint ^ value) * kMixConstant;
}

inline std::uint64_t MixObjc3ParserContractFingerprintString(
    std::uint64_t fingerprint,
    const std::string &value) {
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint, static_cast<std::uint64_t>(value.size()));
  for (const unsigned char c : value) {
    fingerprint = MixObjc3ParserContractFingerprint(
        fingerprint, static_cast<std::uint64_t>(c));
  }
  return fingerprint;
}
