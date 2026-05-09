#pragma once

#include <string>

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/dispatch_abi_marshalling_contracts.h"
#include "lower/contracts/dispatch_surface_classification_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/nil_receiver_semantics_foldability_contracts.h"
#include "lower/contracts/object_model_lowering_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"
#include "lower/contracts/super_dispatch_method_family_contracts.h"

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
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract);

}  // namespace objc3::artifacts::frontend
