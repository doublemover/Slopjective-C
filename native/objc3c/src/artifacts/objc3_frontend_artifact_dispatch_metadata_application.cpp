#include "artifacts/objc3_frontend_artifact_dispatch_metadata_application.h"

namespace objc3::artifacts::frontend {

Objc3DispatchMetadataApplication BuildObjc3DispatchMetadataApplication(
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    const Objc3DispatchControlLoweringSnapshot
        &dispatch_dispatch_control_lowering,
    const Objc3DispatchMetadataPreservationSnapshot
        &dispatch_dispatch_metadata_interface_preservation) {
  Objc3DispatchMetadataApplication application;
  application.dispatch_dispatch_control_lowering_replay_key =
      dispatch_dispatch_control_lowering_replay_key;
  application.dispatch_dispatch_control_lowering =
      dispatch_dispatch_control_lowering;
  application.dispatch_dispatch_metadata_interface_preservation =
      dispatch_dispatch_metadata_interface_preservation;
  return application;
}

Objc3ObjectDispatchMetadataApplication
BuildObjc3ObjectDispatchMetadataApplication(
    const std::string &property_synthesis_ivar_binding_replay_key,
    const Objc3PropertySynthesisIvarBindingSnapshot
        &property_synthesis_ivar_binding,
    const std::string &id_class_sel_object_pointer_typecheck_replay_key,
    const Objc3IdClassSelObjectPointerTypecheckSnapshot
        &id_class_sel_object_pointer_typecheck,
    const std::string &dispatch_surface_classification_replay_key,
    const Objc3DispatchSurfaceClassificationSnapshot
        &dispatch_surface_classification,
    const std::string &message_send_selector_lowering_replay_key,
    const Objc3MessageSendSelectorLoweringSnapshot
        &message_send_selector_lowering,
    const std::string &dispatch_abi_marshalling_replay_key,
    const Objc3DispatchAbiMarshallingSnapshot &dispatch_abi_marshalling,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const Objc3NilReceiverSemanticsFoldabilitySnapshot
        &nil_receiver_semantics_foldability,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3SuperDispatchMethodFamilySnapshot
        &super_dispatch_method_family,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeLinkHostLinkSnapshot &runtime_link_host_link) {
  Objc3ObjectDispatchMetadataApplication application;
  application.property_synthesis_ivar_binding_replay_key =
      property_synthesis_ivar_binding_replay_key;
  application.property_synthesis_ivar_binding =
      property_synthesis_ivar_binding;
  application.id_class_sel_object_pointer_typecheck_replay_key =
      id_class_sel_object_pointer_typecheck_replay_key;
  application.id_class_sel_object_pointer_typecheck =
      id_class_sel_object_pointer_typecheck;
  application.dispatch_surface_classification_replay_key =
      dispatch_surface_classification_replay_key;
  application.dispatch_surface_classification =
      dispatch_surface_classification;
  application.message_send_selector_lowering_replay_key =
      message_send_selector_lowering_replay_key;
  application.message_send_selector_lowering =
      message_send_selector_lowering;
  application.dispatch_abi_marshalling_replay_key =
      dispatch_abi_marshalling_replay_key;
  application.dispatch_abi_marshalling = dispatch_abi_marshalling;
  application.nil_receiver_semantics_foldability_replay_key =
      nil_receiver_semantics_foldability_replay_key;
  application.nil_receiver_semantics_foldability =
      nil_receiver_semantics_foldability;
  application.super_dispatch_method_family_replay_key =
      super_dispatch_method_family_replay_key;
  application.super_dispatch_method_family = super_dispatch_method_family;
  application.runtime_link_host_link_replay_key =
      runtime_link_host_link_replay_key;
  application.runtime_link_host_link = runtime_link_host_link;
  return application;
}

}  // namespace objc3::artifacts::frontend
