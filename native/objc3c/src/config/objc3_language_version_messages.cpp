#include "config/objc3_language_version.h"

namespace objc3c::config {

std::string UnsupportedLanguageVersionDiagnostic(std::uint32_t version) {
  return "unsupported Objective-C language version for native frontend "
         "(expected " +
         std::to_string(static_cast<unsigned>(kCanonicalLanguageVersion)) +
         "): " + std::to_string(version);
}

}  // namespace objc3c::config
