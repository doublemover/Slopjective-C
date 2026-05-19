#include "config/objc3_feature_state_table_contract.h"

#include <set>
#include <string>
#include <string_view>

#include "config/objc3_feature_state_catalog.h"

namespace objc3c::config {
namespace {

void AccumulateFeatureStateContractEntry(
    FeatureStateTableContractSummary &summary,
    const LanguageFeatureState &entry) {
  ++summary.entry_count;
  switch (entry.state) {
    case FeatureState::Implemented:
      ++summary.implemented_count;
      if (entry.feature == std::string_view("objc3-language-version")) {
        summary.canonical_language_version_implemented = true;
      }
      break;
    case FeatureState::Rejected:
      ++summary.rejected_count;
      summary.rejected_entries_have_diagnostic_codes =
          summary.rejected_entries_have_diagnostic_codes &&
          entry.diagnostic_code != nullptr && entry.diagnostic_code[0] != '\0';
      break;
    case FeatureState::Reserved:
      ++summary.reserved_count;
      break;
    case FeatureState::Internal:
      ++summary.internal_count;
      break;
  }
}

void FinalizeFeatureStateContractSummary(
    FeatureStateTableContractSummary &summary,
    std::size_t unique_feature_id_count) {
  summary.feature_ids_unique = unique_feature_id_count == summary.entry_count;
  summary.fail_closed_truth_table =
      summary.feature_ids_unique &&
      summary.canonical_language_version_implemented &&
      summary.rejected_entries_have_diagnostic_codes &&
      summary.rejected_count > 0u;
}

}  // namespace

FeatureStateTableContractSummary BuildFeatureStateTableContractSummary() {
  FeatureStateTableContractSummary summary;
  std::set<std::string> feature_ids;
  summary.rejected_entries_have_diagnostic_codes = true;
  for (const LanguageFeatureState &entry : CanonicalFeatureStates()) {
    feature_ids.insert(std::string(entry.feature));
    AccumulateFeatureStateContractEntry(summary, entry);
  }
  FinalizeFeatureStateContractSummary(summary, feature_ids.size());
  return summary;
}

}  // namespace objc3c::config
