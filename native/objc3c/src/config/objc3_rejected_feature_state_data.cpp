#include "config/objc3_rejected_feature_state_data.h"

#include <array>

namespace objc3c::config {
namespace {

constexpr std::array<LanguageFeatureState, kRejectedFeatureStateDataCount>
    kRejectedFeatureStates = {{
        {"removed-legacy-objective-c-mode", FeatureState::Rejected, "O3C001",
         "Retired legacy Objective-C mode is rejected by active surfaces."},
        {"removed-legacy-literal-spellings", FeatureState::Rejected, "O3C002",
         "YES, NO, and NULL are rejected as retired literal spellings on the canonical path."},
        {"removed-migration-assist-mode", FeatureState::Rejected, "O3C003",
         "Migration-assist behavior is rejected as a removed hard-cutover mode."},
        {"removed-optional-template-spelling", FeatureState::Rejected, "O3C004",
         "optional<T> spellings are rejected in favor of canonical Optional<T> spelling."},
        {"removed-runtime-dispatch-shim", FeatureState::Rejected, "O3R001",
         "Retired runtime shim dispatch switches are rejected."},
        {"removed-runtime-dispatch-fallback", FeatureState::Rejected, "O3R001",
         "Retired runtime fallback dispatch switches are rejected."},
        {"removed-evidence-log-canonical-diagnostics", FeatureState::Rejected,
         "O3C003",
         "Evidence-log canonical rejection diagnostics are rejected as an inactive mode."},
        {"removed-shim-command-options", FeatureState::Rejected, "O3C001",
         "Retired shim switches are rejected by the native command surface."},
        {"removed-old-mode-command-options", FeatureState::Rejected, "O3C001",
         "Old-mode option spellings are rejected instead of normalized."},
    }};

}  // namespace

std::span<const LanguageFeatureState> RejectedFeatureStateData() {
  return std::span<const LanguageFeatureState>(kRejectedFeatureStates.data(),
                                               kRejectedFeatureStates.size());
}

}  // namespace objc3c::config
