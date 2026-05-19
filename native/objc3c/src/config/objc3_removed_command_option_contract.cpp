#include "config/objc3_removed_command_option_contract.h"

#include <set>
#include <string>
#include <string_view>

#include "config/objc3_config_parsing.h"
#include "config/objc3_command_options.h"

namespace objc3c::config {
namespace {

void AccumulateRemovedCommandOptionContractEntry(
    RemovedCommandOptionValidationContractSummary &summary,
    const CommandOptionState &entry) {
  ++summary.entry_count;
  switch (entry.owner) {
    case RemovedCommandOptionOwner::kLanguageMode:
      ++summary.language_mode_count;
      break;
    case RemovedCommandOptionOwner::kReporting:
      ++summary.reporting_count;
      break;
    case RemovedCommandOptionOwner::kRuntime:
      ++summary.runtime_count;
      break;
  }
  if (entry.state == FeatureState::Rejected) {
    ++summary.rejected_count;
  }
  summary.normalized_spellings =
      summary.normalized_spellings &&
      NormalizeCommandOptionKey(entry.spelling) == entry.spelling;
  summary.diagnostics_present =
      summary.diagnostics_present && entry.diagnostic_code != nullptr &&
      entry.diagnostic_code[0] != '\0';
  summary.summaries_present =
      summary.summaries_present && entry.summary != nullptr &&
      entry.summary[0] != '\0';
  summary.contract_ids_match =
      summary.contract_ids_match && entry.contract_id != nullptr &&
      std::string_view(entry.contract_id) ==
          objc3c::contracts::kObjc3RemovedOptionValidationContractId;
}

void FinalizeRemovedCommandOptionValidationContractSummary(
    RemovedCommandOptionValidationContractSummary &summary,
    std::size_t unique_spelling_count) {
  summary.spellings_unique = unique_spelling_count == summary.entry_count;
  summary.owner_tables_present = summary.language_mode_count > 0u &&
                                 summary.reporting_count > 0u &&
                                 summary.runtime_count > 0u;
  summary.fail_closed_removed_option_table =
      summary.entry_count > 0u && summary.spellings_unique &&
      summary.normalized_spellings && summary.diagnostics_present &&
      summary.summaries_present && summary.owner_tables_present &&
      summary.contract_ids_match &&
      summary.rejected_count == summary.entry_count;
}

}  // namespace

RemovedCommandOptionValidationContractSummary
BuildRemovedCommandOptionValidationContractSummary() {
  RemovedCommandOptionValidationContractSummary summary;
  std::set<std::string> spellings;
  summary.normalized_spellings = true;
  summary.diagnostics_present = true;
  summary.summaries_present = true;
  summary.contract_ids_match = true;
  for (const CommandOptionState &entry : RemovedCommandOptions()) {
    spellings.insert(std::string(entry.spelling));
    AccumulateRemovedCommandOptionContractEntry(summary, entry);
  }
  FinalizeRemovedCommandOptionValidationContractSummary(summary,
                                                       spellings.size());
  return summary;
}

}  // namespace objc3c::config
