#include "artifacts/objc3_frontend_artifact_bundle_publication.h"

#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_bundle_outputs.h"
#include "artifacts/objc3_frontend_artifact_conformance_report_plan.h"
#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ir_application.h"
#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"
#include "artifacts/objc3_frontend_artifacts.h"

namespace objc3::artifacts::frontend {

void PublishObjc3FrontendArtifactBundleOutputs(
    const Objc3FrontendArtifactBundlePublicationInputs &inputs) {
  inputs.bundle.manifest_json = inputs.manifest_json;
  inputs.bundle.runtime_metadata_binary =
      inputs.runtime_metadata_plan.executable_metadata_runtime_ingest_binary_payload;

  PopulateObjc3FrontendArtifactBundleOutputs(
      inputs.bundle, inputs.program,
      inputs.runtime_import_plan.runtime_aware_import_module_frontend_closure,
      inputs.pipeline_result.runtime_metadata_source_records,
      inputs.pipeline_result.sema_type_metadata_handoff,
      inputs.core_lowering_plan.type_system_optional_keypath_lowering_contract,
      inputs.type_system_type_semantic_model_summary,
      inputs.core_lowering_plan.message_send_selector_lowering_replay_key,
      inputs.core_lowering_plan.dispatch_abi_marshalling_replay_key,
      inputs.core_lowering_plan.nil_receiver_semantics_foldability_replay_key,
      inputs.core_lowering_plan.type_system_optional_keypath_lowering_replay_key,
      inputs.runtime_registration_plan.runtime_support_library_link_wiring,
      inputs.interop_lowering_plan
          .error_handling_result_and_bridging_artifact_replay_summary,
      inputs.semantic_lowering_plan.concurrency_actor_lowering_metadata_contract,
      inputs.semantic_lowering_plan.concurrency_actor_lowering_metadata_replay_key,
      inputs.semantic_lowering_plan
          .concurrency_actor_isolation_sendability_lowering_replay_key,
      inputs.interop_lowering_plan
          .interop_foreign_surface_interface_preservation_summary,
      inputs.interop_lowering_plan
          .interop_header_module_bridge_generation_summary,
      inputs.interop_lowering_plan
          .interop_foreign_call_lifetime_lowering_contract,
      inputs.interop_lowering_plan
          .interop_foreign_call_lifetime_lowering_replay_key,
      inputs.interop_lowering_plan
          .interop_ffi_metadata_interface_preservation_contract,
      inputs.interop_lowering_plan
          .interop_ffi_metadata_interface_preservation_replay_key,
      inputs.artifact_preservation_plan
          .metaprogramming_module_interface_replay_preservation_summary,
      inputs.artifact_preservation_plan
          .metaprogramming_macro_host_process_cache_runtime_integration_summary,
      inputs.artifact_preservation_plan
          .dispatch_dispatch_metadata_interface_preservation_summary,
      inputs.artifact_preservation_plan
          .runtime_block_ownership_artifact_preservation_summary,
      inputs.artifact_preservation_plan
          .runtime_storage_reflection_artifact_preservation_summary,
      inputs.runtime_import_plan.serialized_runtime_metadata_artifact_reuse,
      inputs.runtime_import_plan.serialized_runtime_metadata_reuse_records,
      inputs.conformance_report_plan.versioned_conformance_report_lowering,
      inputs.options, inputs.pipeline_result,
      inputs.conformance_report_plan
          .frontend_compatibility_strictness_claim_semantics,
      inputs.conformance_report_plan
          .tooling_feature_aware_conformance_report_emission_summary,
      inputs.conformance_report_plan
          .tooling_corpus_sharding_release_evidence_packaging_summary,
      inputs.runtime_registration_plan
          .runtime_registration_descriptor_image_root_source_surface,
      inputs.runtime_registration_plan
          .runtime_registration_descriptor_frontend_closure,
      inputs.runtime_registration_plan
          .runtime_translation_unit_registration_manifest,
      inputs.runtime_registration_plan.runtime_bootstrap_legality_semantics,
      inputs.runtime_registration_plan
          .runtime_bootstrap_legality_failure_contract,
      inputs.runtime_registration_plan
          .runtime_bootstrap_failure_restart_semantics,
      inputs.conformance_report_plan
          .tooling_legacy_canonical_migration_semantics_summary,
      inputs.conformance_report_plan
          .tooling_machine_readable_conformance_report_contract_summary,
      inputs.runtime_registration_plan.runtime_bootstrap_api,
      inputs.runtime_registration_plan.runtime_bootstrap_semantics,
      inputs.runtime_registration_plan.runtime_bootstrap_lowering);

  FinalizeObjc3FrontendArtifactIrApplication({
      .bundle = inputs.bundle,
      .input_path = inputs.input_path,
      .pipeline_result = inputs.pipeline_result,
      .options = inputs.options,
      .program = inputs.program,
      .post_pipeline_failure = inputs.post_pipeline_failure,
      .ir_emission_core_feature_impl_surface =
          inputs.ir_emission_core_feature_impl_surface,
      .conformance_report_plan = inputs.conformance_report_plan,
      .semantic_lowering_plan = inputs.semantic_lowering_plan,
      .core_lowering_plan = inputs.core_lowering_plan,
      .ownership_aware_lowering_plan = inputs.ownership_aware_lowering_plan,
      .block_lowering_plan = inputs.block_lowering_plan,
      .type_system_lowering_plan = inputs.type_system_lowering_plan,
      .runtime_import_plan = inputs.runtime_import_plan,
      .module_lowering_plan = inputs.module_lowering_plan,
      .error_lowering_plan = inputs.error_lowering_plan,
      .interop_lowering_plan = inputs.interop_lowering_plan,
      .artifact_preservation_plan = inputs.artifact_preservation_plan,
      .runtime_metadata_plan = inputs.runtime_metadata_plan,
      .runtime_registration_plan = inputs.runtime_registration_plan});
}

}  // namespace objc3::artifacts::frontend
