#include "config/objc3_feature_state_table_contract_parts.h"

namespace objc3c::config {

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

}  // namespace objc3c::config
