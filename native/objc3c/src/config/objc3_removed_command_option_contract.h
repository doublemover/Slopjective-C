#pragma once

#include <cstddef>

#include "contracts/objc3_removed_option_validation_contract_id.h"

namespace objc3c::config {

struct RemovedCommandOptionValidationContractSummary {
  const char *contract_id =
      objc3c::contracts::kObjc3RemovedOptionValidationContractId;
  const char *owner = "config";
  std::size_t entry_count = 0;
  std::size_t language_mode_count = 0;
  std::size_t reporting_count = 0;
  std::size_t runtime_count = 0;
  std::size_t rejected_count = 0;
  bool spellings_unique = false;
  bool normalized_spellings = false;
  bool diagnostics_present = false;
  bool summaries_present = false;
  bool owner_tables_present = false;
  bool contract_ids_match = false;
  bool fail_closed_removed_option_table = false;
};

RemovedCommandOptionValidationContractSummary
BuildRemovedCommandOptionValidationContractSummary();

}  // namespace objc3c::config
