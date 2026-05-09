#include "config/objc3_language_version.h"

#include "config/objc3_config_parsing.h"

namespace objc3c::config {

bool ParseLanguageVersionText(std::string_view text, std::uint32_t &version) {
  return ParseUnsignedConfigValue(text, version);
}

}  // namespace objc3c::config
