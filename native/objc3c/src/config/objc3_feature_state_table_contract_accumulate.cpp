#include "config/objc3_feature_state_table_contract_parts.h"

#include <string_view>

namespace objc3c::config {

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

}  // namespace objc3c::config
