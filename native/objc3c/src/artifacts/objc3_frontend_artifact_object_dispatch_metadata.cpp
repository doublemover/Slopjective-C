#include "artifacts/objc3_frontend_artifact_object_dispatch_metadata.h"

#include "artifacts/objc3_frontend_artifact_dispatch_contract_snapshots.h"
#include "artifacts/objc3_frontend_artifact_dispatch_metadata_application.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendObjectDispatchMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &property_synthesis_ivar_binding_replay_key,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract,
    const std::string &id_class_sel_object_pointer_typecheck_replay_key,
    const Objc3IdClassSelObjectPointerTypecheckContract
        &id_class_sel_object_pointer_typecheck_contract,
    const std::string &dispatch_surface_classification_replay_key,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const std::string &message_send_selector_lowering_replay_key,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract,
    const std::string &dispatch_abi_marshalling_replay_key,
    const Objc3DispatchAbiMarshallingContract
        &dispatch_abi_marshalling_contract,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const Objc3NilReceiverSemanticsFoldabilityContract
        &nil_receiver_semantics_foldability_contract,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3SuperDispatchMethodFamilyContract
        &super_dispatch_method_family_contract,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract) {
  ApplyObjc3FrontendObjectDispatchMetadataSnapshots(
      ir_frontend_metadata,
      property_synthesis_ivar_binding_replay_key,
      BuildPropertySynthesisIvarBindingSnapshot(
          property_synthesis_ivar_binding_contract),
      id_class_sel_object_pointer_typecheck_replay_key,
      BuildIdClassSelObjectPointerTypecheckSnapshot(
          id_class_sel_object_pointer_typecheck_contract),
      dispatch_surface_classification_replay_key,
      BuildDispatchSurfaceClassificationSnapshot(
          dispatch_surface_classification_contract),
      message_send_selector_lowering_replay_key,
      BuildMessageSendSelectorLoweringSnapshot(
          message_send_selector_lowering_contract),
      dispatch_abi_marshalling_replay_key,
      BuildDispatchAbiMarshallingSnapshot(dispatch_abi_marshalling_contract),
      nil_receiver_semantics_foldability_replay_key,
      BuildNilReceiverSemanticsFoldabilitySnapshot(
          nil_receiver_semantics_foldability_contract),
      super_dispatch_method_family_replay_key,
      BuildSuperDispatchMethodFamilySnapshot(
          super_dispatch_method_family_contract),
      runtime_link_host_link_replay_key,
      BuildRuntimeLinkHostLinkSnapshot(runtime_link_host_link_contract));
}

}  // namespace objc3::artifacts::frontend
