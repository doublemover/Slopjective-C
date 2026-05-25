#include "driver/objc3_cli_conformance_profile.h"

bool ParseObjc3ConformanceProfile(const std::string &value,
                                  Objc3ConformanceProfile &profile) {
  if (value == "core") {
    profile = Objc3ConformanceProfile::kCore;
    return true;
  }
  if (value == "strict") {
    profile = Objc3ConformanceProfile::kStrict;
    return true;
  }
  if (value == "strict-concurrency") {
    profile = Objc3ConformanceProfile::kStrictConcurrency;
    return true;
  }
  if (value == "strict-system") {
    profile = Objc3ConformanceProfile::kStrictSystem;
    return true;
  }
  return false;
}

std::string ConformanceProfileName(Objc3ConformanceProfile profile) {
  switch (profile) {
    case Objc3ConformanceProfile::kCore:
      return "core";
    case Objc3ConformanceProfile::kStrict:
      return "strict";
    case Objc3ConformanceProfile::kStrictConcurrency:
      return "strict-concurrency";
    case Objc3ConformanceProfile::kStrictSystem:
      return "strict-system";
  }
  return "invalid-conformance-profile";
}

std::string ConformanceProfileLanguageProfileName(
    Objc3ConformanceProfile profile) {
  switch (profile) {
    case Objc3ConformanceProfile::kCore:
      return "canonical";
    case Objc3ConformanceProfile::kStrict:
      return "strict";
    case Objc3ConformanceProfile::kStrictConcurrency:
      return "strict-concurrency";
    case Objc3ConformanceProfile::kStrictSystem:
      return "strict-system";
  }
  return "invalid-language-profile";
}
