#include "config/objc3_language_profile_table.h"

#include <array>

namespace objc3c::config {
namespace {

constexpr std::array<LanguageProfileContract, 3> kLanguageProfiles = {{
    {LanguageProfileId::kCanonical,
     kCanonicalLanguageProfileName,
     kCanonicalLanguageVersion,
     objc3c::contracts::kObjc3CanonicalLanguageProfileContractId,
     false,
     false},
    {LanguageProfileId::kStrict,
     kStrictLanguageProfileName,
     kCanonicalLanguageVersion,
     "objc3c.config.language_profile.strict.v1",
     true,
     false},
    {LanguageProfileId::kStrictConcurrency,
     kStrictConcurrencyLanguageProfileName,
     kCanonicalLanguageVersion,
     "objc3c.config.language_profile.strict-concurrency.v1",
     true,
     true},
}};

}  // namespace

std::span<const LanguageProfileContract> LanguageProfileTable() {
  return std::span<const LanguageProfileContract>(kLanguageProfiles.data(),
                                                  kLanguageProfiles.size());
}

}  // namespace objc3c::config
