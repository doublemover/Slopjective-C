#include "config/objc3_feature_state_table_contract.h"

#include <set>
#include <string>

#include "config/objc3_feature_state_catalog.h"
#include "config/objc3_feature_state_table_contract_parts.h"

namespace objc3c::config {

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
