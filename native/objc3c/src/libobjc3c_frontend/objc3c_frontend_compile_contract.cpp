#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"

namespace objc3c::frontend {

bool ValidateSupportedFrontendLanguageVersion(uint8_t requested_language_version,
                                              std::string &error) {
  if (requested_language_version ==
      static_cast<uint8_t>(OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3)) {
    return true;
  }

  error = "invalid compile_options.language_version: " +
          std::to_string(requested_language_version) +
          " (accepted value is Objective-C version 3).";
  return false;
}

}  // namespace objc3c::frontend
