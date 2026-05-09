#pragma once

#include <cstddef>

#include "config/objc3_feature_state_catalog.h"
#include "config/objc3_feature_state_table_contract.h"

namespace objc3c::config {

void AccumulateFeatureStateContractEntry(
    FeatureStateTableContractSummary &summary,
    const LanguageFeatureState &entry);
void FinalizeFeatureStateContractSummary(
    FeatureStateTableContractSummary &summary,
    std::size_t unique_feature_id_count);

}  // namespace objc3c::config
