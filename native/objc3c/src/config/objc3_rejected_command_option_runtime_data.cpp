#include "config/objc3_rejected_command_option_runtime_data.h"

#include <array>

namespace objc3c::config {
namespace {

constexpr std::array<CommandOptionState, kRejectedRuntimeCommandOptionDataCount>
    kRejectedRuntimeCommandOptions = {{
        {"--objc3-runtime-gate-dispatch", RemovedCommandOptionOwner::kRuntime,
         FeatureState::Rejected, "O3R001",
         "Retired runtime gate dispatch flags are rejected; dispatch must be strict and typed."},
        {"--objc3-runtime-retired-route", RemovedCommandOptionOwner::kRuntime,
         FeatureState::Rejected, "O3R001",
         "Retired runtime retired route flags are rejected; unresolved dispatch is a structured error."},
        {"-fobjc3-runtime-dispatch-retired-route",
         RemovedCommandOptionOwner::kRuntime, FeatureState::Rejected,
         "OBJC3-E-REMOVED-RUNTIME-RETIRED_ROUTE",
         "runtime dispatch retired route ABI selection is removed; lowering must target the canonical typed dispatch ABI or fail"},
        {"--objc3-allow-retired-routes", RemovedCommandOptionOwner::kRuntime,
         FeatureState::Rejected, "O3C001",
         "Retired retired-route-enabling command switches are rejected by the canonical frontend."},
        {"--objc3-enable-gates", RemovedCommandOptionOwner::kRuntime,
         FeatureState::Rejected, "O3C001",
         "Retired gate-enabling command switches are rejected by the canonical frontend."},
    }};

}  // namespace

std::span<const CommandOptionState> RejectedRuntimeCommandOptionData() {
  return std::span<const CommandOptionState>(
      kRejectedRuntimeCommandOptions.data(),
      kRejectedRuntimeCommandOptions.size());
}

}  // namespace objc3c::config
