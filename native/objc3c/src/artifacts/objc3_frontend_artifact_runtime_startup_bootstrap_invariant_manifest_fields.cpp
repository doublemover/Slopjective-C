#include "artifacts/objc3_frontend_artifact_runtime_startup_bootstrap_invariant_manifest_fields.h"

#include <ostream>

#include "io/objc3_json.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeStartupBootstrapInvariantManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeStartupBootstrapInvariantSummary
        &runtime_startup_bootstrap_invariants) {
  manifest << ",\"runtime_startup_bootstrap_invariant_contract_id\":\""
           << runtime_startup_bootstrap_invariants.contract_id
           << "\",\"runtime_startup_bootstrap_invariant_duplicate_registration_policy\":\""
           << runtime_startup_bootstrap_invariants
                  .duplicate_registration_policy
           << "\",\"runtime_startup_bootstrap_invariant_realization_order_policy\":\""
           << runtime_startup_bootstrap_invariants.realization_order_policy
           << "\",\"runtime_startup_bootstrap_invariant_failure_mode\":\""
           << runtime_startup_bootstrap_invariants.failure_mode
           << "\",\"runtime_startup_bootstrap_invariant_image_local_initialization_scope\":\""
           << runtime_startup_bootstrap_invariants
                  .image_local_initialization_scope
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_uniqueness_policy\":\""
           << runtime_startup_bootstrap_invariants
                  .constructor_root_uniqueness_policy
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_consumption_model\":\""
           << runtime_startup_bootstrap_invariants
                  .constructor_root_consumption_model
           << "\",\"runtime_startup_bootstrap_invariant_startup_execution_mode\":\""
           << runtime_startup_bootstrap_invariants.startup_execution_mode
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_symbol\":\""
           << runtime_startup_bootstrap_invariants.constructor_root_symbol
           << "\",\"runtime_startup_bootstrap_invariant_registration_entrypoint_symbol\":\""
           << runtime_startup_bootstrap_invariants
                  .registration_entrypoint_symbol
           << "\",\"runtime_startup_bootstrap_invariant_manifest_authority_model\":\""
           << runtime_startup_bootstrap_invariants.manifest_authority_model
           << "\",\"runtime_startup_bootstrap_invariant_translation_unit_identity_model\":\""
           << runtime_startup_bootstrap_invariants
                  .translation_unit_identity_model
           << "\",\"runtime_startup_bootstrap_invariant_fail_closed\":"
           << (runtime_startup_bootstrap_invariants.fail_closed ? "true"
                                                                : "false")
           << ",\"runtime_startup_bootstrap_invariant_registration_manifest_contract_ready\":"
           << (runtime_startup_bootstrap_invariants
                       .registration_manifest_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_duplicate_registration_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .duplicate_registration_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_realization_order_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .realization_order_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_failure_mode_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .failure_mode_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_image_local_initialization_scope_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .image_local_initialization_scope_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_constructor_root_uniqueness_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .constructor_root_uniqueness_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_startup_execution_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .startup_execution_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_live_duplicate_registration_enforcement_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .live_duplicate_registration_enforcement_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_image_local_realization_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .image_local_realization_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_ready_for_bootstrap_implementation\":"
           << (runtime_startup_bootstrap_invariants
                       .ready_for_bootstrap_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_registration_manifest_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_startup_bootstrap_invariants
                      .registration_manifest_replay_key)
           << "\",\"runtime_startup_bootstrap_invariant_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_startup_bootstrap_invariants.replay_key)
           << "\",\"runtime_startup_bootstrap_invariant_failure_reason\":\""
           << objc3::io::EscapeJsonString(
                  runtime_startup_bootstrap_invariants.failure_reason)
           << "\"";
}

}  // namespace objc3::artifacts::frontend
