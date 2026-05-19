#include "config/objc3_rejected_command_option_language_data.h"

#include <array>

namespace objc3c::config {
namespace {

constexpr std::array<CommandOptionState,
                     kRejectedLanguageModeCommandOptionDataCount>
    kRejectedLanguageModeCommandOptions = {{
        {"--objc2-compatible", RemovedCommandOptionOwner::kLanguageMode,
         FeatureState::Rejected, "O3C001",
         "Retired mode flags are rejected; Objective-C 3.0 is canonical-only."},
        {"--objc3-compat-mode", RemovedCommandOptionOwner::kLanguageMode,
         FeatureState::Rejected, "O3C001",
         "Retired compat-mode flags are rejected; Objective-C 3.0 is canonical-only."},
        {"--objc3-legacy-mode", RemovedCommandOptionOwner::kLanguageMode,
         FeatureState::Rejected, "O3C001",
         "Legacy Objective-C mode spellings are removed from the native command surface."},
        {"--objc3-old-mode", RemovedCommandOptionOwner::kLanguageMode,
         FeatureState::Rejected, "O3C001",
         "Old-mode spellings are rejected instead of normalized."},
        {"-fobjc3-parser-retired-route",
         RemovedCommandOptionOwner::kLanguageMode, FeatureState::Rejected,
         "OBJC3-E-REMOVED-RETIRED_ROUTE-FLAG",
         "parser retired route feature toggles are removed; parser recovery must fail closed with canonical diagnostics"},
        {"-fobjc3-compatibility-gates",
         RemovedCommandOptionOwner::kLanguageMode, FeatureState::Rejected,
         "OBJC3-E-REMOVED-COMPATIBILITY-GATE",
         "compatibility gates are removed; Objective-C 3 native mode is canonical-only and fail-closed"},
        {"--objc3-migration-assist", RemovedCommandOptionOwner::kLanguageMode,
         FeatureState::Rejected, "O3C003",
         "Migration-assist mode was removed from the hard-cutover command surface."},
    }};

}  // namespace

std::span<const CommandOptionState> RejectedLanguageModeCommandOptionData() {
  return std::span<const CommandOptionState>(
      kRejectedLanguageModeCommandOptions.data(),
      kRejectedLanguageModeCommandOptions.size());
}

}  // namespace objc3c::config
