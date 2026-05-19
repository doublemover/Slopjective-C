#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"

#include <sstream>
#include <string>

#include "artifacts/objc3_runtime_bootstrap_contract_artifact_builders.h"

namespace objc3::artifacts::frontend {

std::string BuildRuntimeStartupBootstrapInvariantReplayKey(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  return runtime_bootstrap_contracts::
      RuntimeStartupBootstrapInvariantArtifactBuilder::BuildReplayKey(summary);
}

Objc3RuntimeStartupBootstrapInvariantSummary
BuildRuntimeStartupBootstrapInvariantSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  return runtime_bootstrap_contracts::
      RuntimeStartupBootstrapInvariantArtifactBuilder::BuildSummary(
          registration_manifest);
}

std::string BuildRuntimeStartupBootstrapInvariantSummaryJson(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  return runtime_bootstrap_contracts::
      RuntimeStartupBootstrapInvariantArtifactBuilder::BuildSummaryJson(
          summary);
}

std::string BuildRuntimeBootstrapSemanticsReplayKey(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  return runtime_bootstrap_contracts::
      RuntimeBootstrapSemanticsArtifactBuilder::BuildReplayKey(summary);
}

Objc3RuntimeBootstrapSemanticsSummary BuildRuntimeBootstrapSemanticsSummary(
    const Objc3RuntimeStartupBootstrapInvariantSummary &bootstrap_invariants,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  return runtime_bootstrap_contracts::
      RuntimeBootstrapSemanticsArtifactBuilder::BuildSummary(
          bootstrap_invariants,
          registration_manifest);
}

std::string BuildRuntimeBootstrapSemanticsSummaryJson(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  return runtime_bootstrap_contracts::
      RuntimeBootstrapSemanticsArtifactBuilder::BuildSummaryJson(summary);
}

std::string BuildRuntimeBootstrapLoweringReplayKey(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";registration_descriptor_frontend_closure_contract_id="
      << summary.registration_descriptor_frontend_closure_contract_id
      << ";registration_descriptor_artifact="
      << summary.registration_descriptor_artifact
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";lowering_boundary_model=" << summary.lowering_boundary_model
      << ";registration_descriptor_handoff_model="
      << summary.registration_descriptor_handoff_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";registration_table_symbol_prefix="
      << summary.registration_table_symbol_prefix
      << ";image_local_init_state_symbol_prefix="
      << summary.image_local_init_state_symbol_prefix
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";global_ctor_list_model=" << summary.global_ctor_list_model
      << ";registration_table_layout_model="
      << summary.registration_table_layout_model
      << ";image_local_initialization_model="
      << summary.image_local_initialization_model
      << ";registration_table_abi_version="
      << summary.registration_table_abi_version
      << ";registration_table_pointer_field_count="
      << summary.registration_table_pointer_field_count
      << ";constructor_root_emission_state="
      << summary.constructor_root_emission_state
      << ";init_stub_emission_state=" << summary.init_stub_emission_state
      << ";registration_table_emission_state="
      << summary.registration_table_emission_state
      << ";bootstrap_ir_materialization_landed="
      << (summary.bootstrap_ir_materialization_landed ? "true" : "false")
      << ";image_local_initialization_landed="
      << (summary.image_local_initialization_landed ? "true" : "false")
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key
      << ";registration_descriptor_frontend_closure_replay_key="
      << summary.registration_descriptor_frontend_closure_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapLoweringSummary BuildRuntimeBootstrapLoweringSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure) {
  Objc3RuntimeBootstrapLoweringSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.registration_descriptor_frontend_closure_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          registration_descriptor_frontend_closure);
  summary.lowering_contract_published = true;
  summary.manifest_authority_preserved =
      summary.registration_manifest_contract_ready &&
      registration_manifest.constructor_root_manifest_authoritative;
  summary.no_bootstrap_ir_materialization_yet = false;
  summary.bootstrap_ir_materialization_landed =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  summary.image_local_initialization_landed =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  summary.ready_for_bootstrap_materialization =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.bootstrap_semantics_contract_ready) {
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }
  if (summary.registration_descriptor_frontend_closure_contract_ready) {
    summary.registration_descriptor_frontend_closure_replay_key =
        registration_descriptor_frontend_closure.replay_key;
  }
  if (summary.ready_for_bootstrap_materialization) {
    summary.replay_key = BuildRuntimeBootstrapLoweringReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapLoweringSummary(summary)) {
    summary.failure_reason = "runtime bootstrap lowering summary is incomplete";
  }
  return summary;
}

}  // namespace objc3::artifacts::frontend
