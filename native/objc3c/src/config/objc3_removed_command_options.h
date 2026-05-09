#pragma once

#include <array>
#include <string>

#include "config/objc3_feature_state_catalog.h"

namespace objc3c::config {

struct CommandOptionState {
  const char *spelling;
  FeatureState state;
  const char *diagnostic_code;
  const char *summary;
};

inline constexpr std::array<CommandOptionState, 3> kRemovedCommandOptions = {{
    {"--objc3-compat-mode", FeatureState::Rejected, "O3C001",
     "Objective-C 3.0 is canonical-only; compatibility mode was removed."},
    {"--objc3-migration-assist", FeatureState::Rejected, "O3C003",
     "Migration-assist mode was removed from the hard-cutover command surface."},
    {"--objc3-canonical-rejection-diagnostics", FeatureState::Rejected, "O3C003",
     "Report-only canonical rejection diagnostics were removed from the active command surface."},
}};

inline const CommandOptionState *FindRemovedCommandOption(
    const std::string &flag) {
  for (const auto &option : kRemovedCommandOptions) {
    if (flag == option.spelling) {
      return &option;
    }
  }
  return nullptr;
}

}  // namespace objc3c::config
