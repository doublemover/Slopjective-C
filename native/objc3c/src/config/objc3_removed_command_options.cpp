#include "config/objc3_removed_command_options.h"

#include <array>
#include <cstddef>
#include <string>

#include "config/objc3_config_parsing.h"
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

std::span<const CommandOptionState> RemovedCommandOptionEntries() {
  return std::span<const CommandOptionState>(
      kRemovedCommandOptionEntries.data(), kRemovedCommandOptionEntries.size());
}

}  // namespace

std::span<const CommandOptionState> RemovedCommandOptions() {
  return RemovedCommandOptionEntries();
}

const CommandOptionState *FindRemovedCommandOption(std::string_view flag) {
  const std::string key = NormalizeCommandOptionKey(flag);
  for (const auto &option : RemovedCommandOptionEntries()) {
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
