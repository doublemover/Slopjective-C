#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_release_candidate_evidence_state_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_release_candidate_evidence_state_for_testing(&snapshot);

  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "validation_artifact_ready=" << snapshot.validation_artifact_ready
            << "\n";
  std::cout << "release_evidence_operation_ready="
            << snapshot.release_evidence_operation_ready << "\n";
  std::cout << "dashboard_status_ready=" << snapshot.dashboard_status_ready
            << "\n";
  std::cout << "advanced_feature_gate_ready="
            << snapshot.advanced_feature_gate_ready << "\n";
  std::cout << "release_candidate_matrix_ready="
            << snapshot.release_candidate_matrix_ready << "\n";
  std::cout << "deprecated_paths_shutdown="
            << snapshot.deprecated_paths_shutdown << "\n";
  std::cout << "deterministic=" << snapshot.deterministic << "\n";
  std::cout << "validation_artifact_name="
            << (snapshot.validation_artifact_name != nullptr
                    ? snapshot.validation_artifact_name
                    : "")
            << "\n";
  std::cout << "release_evidence_operation_artifact_name="
            << (snapshot.release_evidence_operation_artifact_name != nullptr
                    ? snapshot.release_evidence_operation_artifact_name
                    : "")
            << "\n";
  std::cout << "dashboard_status_artifact_name="
            << (snapshot.dashboard_status_artifact_name != nullptr
                    ? snapshot.dashboard_status_artifact_name
                    : "")
            << "\n";
  std::cout << "advanced_feature_gate_artifact_name="
            << (snapshot.advanced_feature_gate_artifact_name != nullptr
                    ? snapshot.advanced_feature_gate_artifact_name
                    : "")
            << "\n";
  std::cout << "release_candidate_matrix_artifact_name="
            << (snapshot.release_candidate_matrix_artifact_name != nullptr
                    ? snapshot.release_candidate_matrix_artifact_name
                    : "")
            << "\n";
  std::cout << "validation_model="
            << (snapshot.validation_model != nullptr ? snapshot.validation_model
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

  return (copy_status == 0 && snapshot.validation_artifact_ready == 1 &&
          snapshot.release_evidence_operation_ready == 1 &&
          snapshot.dashboard_status_ready == 1 &&
          snapshot.advanced_feature_gate_ready == 1 &&
          snapshot.release_candidate_matrix_ready == 1 &&
          snapshot.deprecated_paths_shutdown == 1 &&
          snapshot.deterministic == 1)
             ? 0
             : 1;
}
