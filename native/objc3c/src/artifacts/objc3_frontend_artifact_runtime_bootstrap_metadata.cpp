#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_metadata.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "runtime/metadata/selector_metadata_bootstrap_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3::artifacts::frontend {
namespace {

inline constexpr const char
    *kArtifactRuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId =
        "objc3c.runtime.registration.descriptor.and.image.root.lowering.v1";

}  // namespace

void ApplyObjc3FrontendRuntimeBootstrapMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest) {
  ir_frontend_metadata.runtime_bootstrap_lowering_contract_id =
      runtime_bootstrap_lowering.contract_id;
  ir_frontend_metadata.runtime_bootstrap_lowering_boundary_model =
      runtime_bootstrap_lowering.lowering_boundary_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol =
      runtime_bootstrap_lowering.constructor_root_symbol;
  ir_frontend_metadata.runtime_bootstrap_lowering_init_stub_symbol_prefix =
      runtime_bootstrap_lowering.constructor_init_stub_symbol_prefix;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_symbol_prefix =
      runtime_bootstrap_lowering.registration_table_symbol_prefix;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix =
      runtime_bootstrap_lowering.image_local_init_state_symbol_prefix;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_entrypoint_symbol =
      runtime_bootstrap_lowering.registration_entrypoint_symbol;
  ir_frontend_metadata.runtime_bootstrap_lowering_global_ctor_list_model =
      runtime_bootstrap_lowering.global_ctor_list_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_table_layout_model =
      runtime_bootstrap_lowering.registration_table_layout_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_image_local_initialization_model =
      runtime_bootstrap_lowering.image_local_initialization_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_table_abi_version =
      runtime_bootstrap_lowering.registration_table_abi_version;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_pointer_field_count =
      runtime_bootstrap_lowering.registration_table_pointer_field_count;
  ir_frontend_metadata.runtime_bootstrap_lowering_constructor_root_emission_state =
      runtime_bootstrap_lowering.constructor_root_emission_state;
  ir_frontend_metadata.runtime_bootstrap_lowering_init_stub_emission_state =
      runtime_bootstrap_lowering.init_stub_emission_state;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_emission_state =
      runtime_bootstrap_lowering.registration_table_emission_state;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_bootstrap_ir_materialization_landed =
      runtime_bootstrap_lowering.bootstrap_ir_materialization_landed;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_image_local_initialization_landed =
      runtime_bootstrap_lowering.image_local_initialization_landed;
  ir_frontend_metadata
      .runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id =
      kArtifactRuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId;
  ir_frontend_metadata.runtime_bootstrap_registration_descriptor_identifier =
      runtime_registration_descriptor_frontend_closure
          .registration_descriptor_identifier;
  ir_frontend_metadata.runtime_bootstrap_image_root_identifier =
      runtime_registration_descriptor_frontend_closure.image_root_identifier;
  ir_frontend_metadata.runtime_bootstrap_registration_order_ordinal =
      runtime_translation_unit_registration_manifest
          .translation_unit_registration_order_ordinal;
  ir_frontend_metadata.runtime_bootstrap_lowering_ready =
      runtime_bootstrap_lowering.ready_for_bootstrap_materialization;
  ir_frontend_metadata.runtime_bootstrap_lowering_fail_closed =
      runtime_bootstrap_lowering.fail_closed;
}

}  // namespace objc3::artifacts::frontend
