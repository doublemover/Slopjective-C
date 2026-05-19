#include "io/objc3_runtime_registration_manifest_document.h"

#include "io/objc3_runtime_registration_manifest_document_bootstrap.h"
#include "io/objc3_runtime_registration_manifest_document_header.h"
#include "io/objc3_process_internal.h"
#include "io/objc3_runtime_registration_manifest_sections.h"

std::string BuildObjc3RuntimeTranslationUnitRegistrationManifestDocumentJson(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record,
    std::size_t runtime_metadata_binary_byte_count) {
  // bootstrap-invariant anchor: later startup registration must
  // preserve one init-stub/root identity per translation unit, reject
  // duplicate registration on the same identity key, and fail closed before
  // user entry if bootstrap materialization cannot honor that contract.
  // live bootstrap semantics anchor: the emitted registration
  // manifest now carries the exact duplicate-registration, order, failure-mode,
  // and runtime-status-code contract consumed by the real runtime library and
  // probe harness. Drift between the emitted manifest and runtime behavior must
  // fail closed before later constructor-root automation lands.
  // bootstrap-lowering anchor: the emitted registration manifest now
  // also freezes the lowering-owned ctor-root/init-stub/registration-table
  // materialization boundary. This artifact may publish the canonical names
  // and non-goal states, but it may not synthesize bootstrap globals on its
  // own ahead of the later lowering implementation issue.
  // constructor/init-stub emission anchor: once lowering emits the
  // real bootstrap globals, this manifest must publish the exact derived
  // init-stub and registration-table symbols from the full translation-unit
  // identity key, not a truncated semicolon-split fragment.
  // registration-table/image-local-init anchor: once lowering
  // expands that emitted boundary, the manifest must also publish the
  // self-describing registration-table layout contract and exact derived
  // image-local init-state symbol from the same translation-unit identity key.
  // runtime-bootstrap-api anchor: the same manifest also freezes the
  // runtime-owned bootstrap header/archive/entrypoint/reset surface so later
  // registrar/image-walk work consumes one canonical API contract instead of
  // re-deriving launch-path behavior from scattered runtime details.
  // launch-integration anchor: compile, proof, and execution-smoke
  // command surfaces must all consume this emitted registration manifest as the
  // authoritative runtime launch contract instead of guessing archive paths or
  // linker flags from implicit heuristics.
  // live catch/bridge/runtime integration anchor: runnable Part 6
  // probes keep using this same emitted runtime-library archive path and
  // linker-response topology; lane-D must prove linked error/bridge execution
  // through the packaged runtime rather than inventing a special-case driver
  // path for throws or catch handling.
  // startup-registration gate anchor: lane-E closes over this same
  // emitted manifest plus the replay-stable bootstrap evidence chain from
  // A002/B002/C003/D003/D004, so drift here must fail closed before E002.
  // runbook-closeout anchor: the operator runbook must stay bound to this emitted launch contract
  // and prove the same integrated path end to end.

  std::ostringstream out;
  EmitObjc3RuntimeRegistrationManifestHeaderJson(
      out, inputs, symbol_owner_record, runtime_metadata_binary_byte_count);
  AppendObjc3RuntimeRegistrationManifestAccessorAbiSurfacesJson(out, inputs);
  out << "  \"registration_descriptor_source_contract_id\": \""
      << EscapeJsonString(inputs.registration_descriptor_source_contract_id)
      << "\",\n"
      << "  \"registration_descriptor_source_surface_path\": \""
      << EscapeJsonString(inputs.registration_descriptor_source_surface_path)
      << "\",\n"
      << "  \"registration_descriptor_pragma_name\": \""
      << EscapeJsonString(inputs.registration_descriptor_pragma_name)
      << "\",\n"
      << "  \"image_root_pragma_name\": \""
      << EscapeJsonString(inputs.image_root_pragma_name) << "\",\n"
      << "  \"module_identity_source\": \""
      << EscapeJsonString(inputs.module_identity_source) << "\",\n"
      << "  \"registration_descriptor_identifier\": \""
      << EscapeJsonString(inputs.registration_descriptor_identifier)
      << "\",\n"
      << "  \"registration_descriptor_identity_source\": \""
      << EscapeJsonString(inputs.registration_descriptor_identity_source)
      << "\",\n"
      << "  \"image_root_identifier\": \""
      << EscapeJsonString(inputs.image_root_identifier) << "\",\n"
      << "  \"image_root_identity_source\": \""
      << EscapeJsonString(inputs.image_root_identity_source) << "\",\n"
      << "  \"bootstrap_visible_metadata_ownership_model\": \""
      << EscapeJsonString(inputs.bootstrap_visible_metadata_ownership_model)
      << "\",\n"
      << "  \"class_descriptor_count\": " << inputs.class_descriptor_count
      << ",\n"
      << "  \"protocol_descriptor_count\": "
      << inputs.protocol_descriptor_count << ",\n"
      << "  \"category_descriptor_count\": "
      << inputs.category_descriptor_count << ",\n"
      << "  \"property_descriptor_count\": "
      << inputs.property_descriptor_count << ",\n"
      << "  \"ivar_descriptor_count\": " << inputs.ivar_descriptor_count
      << ",\n"
      << "  \"total_descriptor_count\": " << inputs.total_descriptor_count
      << ",\n"
      << "  \"bootstrap_semantics_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_semantics_contract_id) << "\",\n"
      << "  \"duplicate_registration_policy\": \""
      << EscapeJsonString(inputs.duplicate_registration_policy) << "\",\n"
      << "  \"realization_order_policy\": \""
      << EscapeJsonString(inputs.realization_order_policy) << "\",\n"
      << "  \"failure_mode\": \"" << EscapeJsonString(inputs.failure_mode)
      << "\",\n"
      << "  \"registration_result_model\": \""
      << EscapeJsonString(inputs.registration_result_model) << "\",\n"
      << "  \"registration_order_ordinal_model\": \""
      << EscapeJsonString(inputs.registration_order_ordinal_model) << "\",\n"
      << "  \"runtime_state_snapshot_symbol\": \""
      << EscapeJsonString(inputs.runtime_state_snapshot_symbol) << "\",\n";
  EmitObjc3RuntimeRegistrationManifestBootstrapJson(out,
                                                    inputs,
                                                    linker_retention_artifacts,
                                                    symbol_owner_record);
  return out.str();
}
