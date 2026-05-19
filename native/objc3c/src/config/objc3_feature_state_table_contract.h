#pragma once

#include <cstddef>

namespace objc3c::config {

struct FeatureStateTableContractSummary {
  std::size_t entry_count = 0;
  std::size_t implemented_count = 0;
  std::size_t rejected_count = 0;
  std::size_t reserved_count = 0;
  std::size_t internal_count = 0;
  bool feature_ids_unique = false;
  bool rejected_entries_have_diagnostic_codes = false;
  bool canonical_language_version_implemented = false;
  bool fail_closed_truth_table = false;
};

FeatureStateTableContractSummary BuildFeatureStateTableContractSummary();

}  // namespace objc3c::config
