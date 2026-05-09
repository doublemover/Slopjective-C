#include "config/objc3_config_validation.h"

#include <utility>

namespace objc3c::config {

ConfigValidationResult MakeConfigValidationInvalid(std::string diagnostic_code,
                                                   std::string message) {
  ConfigValidationResult result;
  result.status = ConfigValidationStatus::Invalid;
  result.diagnostic_code = std::move(diagnostic_code);
  result.message = std::move(message);
  return result;
}

}  // namespace objc3c::config
