#include "config/objc3_removed_command_options.h"

#include <array>
#include <string>

#include "config/objc3_config_parsing.h"

namespace objc3c::config {

namespace {

constexpr std::array<CommandOptionState, 10> kRemovedCommandOptions = {{
    {"--objc3-compat-mode", FeatureState::Rejected, "O3C001",
     "Retired compat-mode flags are rejected; Objective-C 3.0 is canonical-only."},
    {"--objc3-legacy-mode", FeatureState::Rejected, "O3C001",
     "Legacy Objective-C mode aliases are removed from the native command surface."},
    {"--objc3-old-mode", FeatureState::Rejected, "O3C001",
     "Old-mode aliases are rejected instead of normalized."},
    {"--objc3-migration-assist", FeatureState::Rejected, "O3C003",
     "Migration-assist mode was removed from the hard-cutover command surface."},
    {"--objc3-canonical-rejection-diagnostics", FeatureState::Rejected,
     "O3C003",
     "Report-only canonical rejection diagnostics were removed from the active command surface."},
    {"--objc3-report-only", FeatureState::Rejected, "O3C003",
     "Report-only diagnostics are retired from the active command surface."},
    {"--objc3-runtime-shim-dispatch", FeatureState::Rejected, "O3R001",
     "Retired runtime shim dispatch flags are rejected; dispatch must be strict and typed."},
    {"--objc3-runtime-fallback", FeatureState::Rejected, "O3R001",
     "Retired runtime fallback flags are rejected; unresolved dispatch is a structured error."},
    {"--objc3-allow-fallbacks", FeatureState::Rejected, "O3C001",
     "Retired fallback-enabling command switches are rejected by the canonical frontend."},
    {"--objc3-enable-shims", FeatureState::Rejected, "O3C001",
     "Retired shim-enabling command switches are rejected by the canonical frontend."},
}};

}  // namespace

std::span<const CommandOptionState> RemovedCommandOptions() {
  return std::span<const CommandOptionState>(kRemovedCommandOptions.data(),
                                             kRemovedCommandOptions.size());
}

const CommandOptionState *FindRemovedCommandOption(std::string_view flag) {
  const std::string key = NormalizeCommandOptionKey(flag);
  for (const auto &option : kRemovedCommandOptions) {
    if (key == option.spelling) {
      return &option;
    }
  }
  return nullptr;
}

ConfigValidationResult ValidateRemovedCommandOption(std::string_view flag) {
  const CommandOptionState *option = FindRemovedCommandOption(flag);
  if (option == nullptr) {
    return MakeConfigValidationAccepted();
  }
  return MakeConfigValidationRejected(
      option->diagnostic_code,
      "unsupported hard-cutover option: " + std::string(flag) +
          " was removed; " + option->summary);
}

}  // namespace objc3c::config
