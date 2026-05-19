#include "config/objc3_rejected_command_option_reporting_data.h"

#include <array>

namespace objc3c::config {
namespace {

constexpr std::array<CommandOptionState,
                     kRejectedReportingCommandOptionDataCount>
    kRejectedReportingCommandOptions = {{
        {"--objc3-canonical-rejection-diagnostics",
         RemovedCommandOptionOwner::kReporting, FeatureState::Rejected,
         "O3C003",
         "Evidence-log canonical rejection diagnostics were removed from the active command surface."},
        {"--objc3-evidence-log", RemovedCommandOptionOwner::kReporting,
         FeatureState::Rejected, "O3C003",
         "Evidence-log diagnostics are retired from the active command surface."},
    }};

}  // namespace

std::span<const CommandOptionState> RejectedReportingCommandOptionData() {
  return std::span<const CommandOptionState>(
      kRejectedReportingCommandOptions.data(),
      kRejectedReportingCommandOptions.size());
}

}  // namespace objc3c::config
