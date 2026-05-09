#include "config/objc3_rejected_command_option_runtime_data.h"

#include <array>

namespace objc3c::config {
namespace {

constexpr std::array<CommandOptionState, kRejectedRuntimeCommandOptionDataCount>
    kRejectedRuntimeCommandOptions = {{
        {"--objc3-runtime-shim-dispatch", RemovedCommandOptionOwner::kRuntime,
         FeatureState::Rejected, "O3R001",
         "Retired runtime shim dispatch flags are rejected; dispatch must be strict and typed."},
        {"--objc3-runtime-fallback", RemovedCommandOptionOwner::kRuntime,
         FeatureState::Rejected, "O3R001",
         "Retired runtime fallback flags are rejected; unresolved dispatch is a structured error."},
        {"--objc3-allow-fallbacks", RemovedCommandOptionOwner::kRuntime,
         FeatureState::Rejected, "O3C001",
         "Retired fallback-enabling command switches are rejected by the canonical frontend."},
        {"--objc3-enable-shims", RemovedCommandOptionOwner::kRuntime,
         FeatureState::Rejected, "O3C001",
         "Retired shim-enabling command switches are rejected by the canonical frontend."},
    }};

}  // namespace

std::span<const CommandOptionState> RejectedRuntimeCommandOptionData() {
  return std::span<const CommandOptionState>(
      kRejectedRuntimeCommandOptions.data(),
      kRejectedRuntimeCommandOptions.size());
}

}  // namespace objc3c::config
