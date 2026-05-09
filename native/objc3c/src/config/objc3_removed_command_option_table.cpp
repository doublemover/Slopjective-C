#include "config/objc3_removed_command_option_table.h"

#include <array>
#include <cstddef>

#include "config/objc3_rejected_command_option_language_data.h"
#include "config/objc3_rejected_command_option_reporting_data.h"
#include "config/objc3_rejected_command_option_runtime_data.h"

namespace objc3c::config {
namespace {

inline constexpr std::size_t kRemovedCommandOptionEntryCount =
    kRejectedLanguageModeCommandOptionDataCount +
    kRejectedReportingCommandOptionDataCount +
    kRejectedRuntimeCommandOptionDataCount;

void AppendCommandOptions(
    std::array<CommandOptionState, kRemovedCommandOptionEntryCount> &entries,
    std::size_t &next, std::span<const CommandOptionState> source) {
  for (const CommandOptionState &entry : source) {
    entries[next++] = entry;
  }
}

std::array<CommandOptionState, kRemovedCommandOptionEntryCount>
BuildRemovedCommandOptionEntries() {
  std::array<CommandOptionState, kRemovedCommandOptionEntryCount> entries{};
  std::size_t next = 0;
  AppendCommandOptions(entries, next,
                       RejectedLanguageModeCommandOptionData());
  AppendCommandOptions(entries, next, RejectedReportingCommandOptionData());
  AppendCommandOptions(entries, next, RejectedRuntimeCommandOptionData());
  return entries;
}

const std::array<CommandOptionState, kRemovedCommandOptionEntryCount>
    kRemovedCommandOptionEntries = BuildRemovedCommandOptionEntries();

}  // namespace

std::span<const CommandOptionState> RemovedCommandOptionTable() {
  return std::span<const CommandOptionState>(
      kRemovedCommandOptionEntries.data(), kRemovedCommandOptionEntries.size());
}

}  // namespace objc3c::config
