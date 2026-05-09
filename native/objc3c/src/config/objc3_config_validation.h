#pragma once

#include <cstdint>
#include <string>

namespace objc3c::config {

enum class ConfigValidationStatus : std::uint8_t {
  Accepted,
  Rejected,
  Invalid,
};

struct ConfigValidationResult {
  ConfigValidationStatus status = ConfigValidationStatus::Accepted;
  std::string diagnostic_code;
  std::string message;

  [[nodiscard]] bool ok() const {
    return status == ConfigValidationStatus::Accepted;
  }
};

ConfigValidationResult MakeConfigValidationAccepted();
ConfigValidationResult MakeConfigValidationRejected(std::string diagnostic_code,
                                                    std::string message);
ConfigValidationResult MakeConfigValidationInvalid(std::string diagnostic_code,
                                                   std::string message);

}  // namespace objc3c::config
