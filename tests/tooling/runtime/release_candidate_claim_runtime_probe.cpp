#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_release_candidate_claim_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_release_candidate_claim_snapshot_for_testing(&snapshot);

  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "claim_bundle_ready=" << snapshot.claim_bundle_ready << "\n";
  std::cout << "deterministic=" << snapshot.deterministic << "\n";
  std::cout << "selected_profile="
            << (snapshot.selected_profile != nullptr ? snapshot.selected_profile
                                                     : "")
            << "\n";
  std::cout << "claimed_profile_ids_csv="
            << (snapshot.claimed_profile_ids_csv != nullptr
                    ? snapshot.claimed_profile_ids_csv
                    : "")
            << "\n";
  std::cout << "targeted_profile_ids_csv="
            << (snapshot.targeted_profile_ids_csv != nullptr
                    ? snapshot.targeted_profile_ids_csv
                    : "")
            << "\n";
  std::cout << "conformance_publication_contract_id="
            << (snapshot.conformance_publication_contract_id != nullptr
                    ? snapshot.conformance_publication_contract_id
                    : "")
            << "\n";
  std::cout << "conformance_claim_operations_contract_id="
            << (snapshot.conformance_claim_operations_contract_id != nullptr
                    ? snapshot.conformance_claim_operations_contract_id
                    : "")
            << "\n";
  std::cout << "release_evidence_operation_contract_id="
            << (snapshot.release_evidence_operation_contract_id != nullptr
                    ? snapshot.release_evidence_operation_contract_id
                    : "")
            << "\n";
  std::cout << "dashboard_status_publication_contract_id="
            << (snapshot.dashboard_status_publication_contract_id != nullptr
                    ? snapshot.dashboard_status_publication_contract_id
                    : "")
            << "\n";
  std::cout << "release_candidate_matrix_contract_id="
            << (snapshot.release_candidate_matrix_contract_id != nullptr
                    ? snapshot.release_candidate_matrix_contract_id
                    : "")
            << "\n";
  std::cout << "dashboard_schema_path="
            << (snapshot.dashboard_schema_path != nullptr
                    ? snapshot.dashboard_schema_path
                    : "")
            << "\n";
  std::cout << "gate_script_path="
            << (snapshot.gate_script_path != nullptr ? snapshot.gate_script_path
                                                     : "")
            << "\n";
  std::cout << "runbook_reference_path="
            << (snapshot.runbook_reference_path != nullptr
                    ? snapshot.runbook_reference_path
                    : "")
            << "\n";
  std::cout << "release_bundle_model="
            << (snapshot.release_bundle_model != nullptr
                    ? snapshot.release_bundle_model
                    : "")
            << "\n";
  std::cout << "deprecated_path_shutdown_model="
            << (snapshot.deprecated_path_shutdown_model != nullptr
                    ? snapshot.deprecated_path_shutdown_model
                    : "")
            << "\n";

  return (copy_status == 0 && snapshot.claim_bundle_ready == 1 &&
          snapshot.deterministic == 1)
             ? 0
             : 1;
}
