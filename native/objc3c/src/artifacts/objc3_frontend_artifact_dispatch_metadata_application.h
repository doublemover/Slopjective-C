#pragma once

#include <string>

#include "artifacts/objc3_frontend_artifact_dispatch_contract_snapshots.h"

struct Objc3IRFrontendMetadata;

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendDispatchMetadataSnapshots(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    const Objc3DispatchControlLoweringSnapshot
        &dispatch_dispatch_control_lowering,
    const Objc3DispatchMetadataPreservationSnapshot
        &dispatch_dispatch_metadata_interface_preservation);

void ApplyObjc3FrontendObjectDispatchMetadataSnapshots(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
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
    const Objc3RuntimeLinkHostLinkSnapshot &runtime_link_host_link);

}  // namespace objc3::artifacts::frontend
