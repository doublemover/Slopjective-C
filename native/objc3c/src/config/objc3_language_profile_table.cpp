#include "config/objc3_language_profile_table.h"

#include <array>

namespace objc3c::config {
namespace {

constexpr std::array<LanguageProfileContract, 1> kLanguageProfiles = {{
    {},
}};

}  // namespace

std::span<const LanguageProfileContract> LanguageProfileTable() {
  return std::span<const LanguageProfileContract>(kLanguageProfiles.data(),
                                                  kLanguageProfiles.size());
}

}  // namespace objc3c::config
