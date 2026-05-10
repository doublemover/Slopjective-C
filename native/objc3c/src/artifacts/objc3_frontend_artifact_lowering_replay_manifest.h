#pragma once

#include <initializer_list>
#include <iosfwd>
#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

struct LoweringReplayManifestEntry {
  std::string manifest_key;
  std::string replay_key;
  std::string contract_id;
  bool deterministic = false;
  std::string contract_field_name = "lane_contract";
};

void WriteLoweringReplayManifestEntries(
    std::ostream &manifest,
    std::initializer_list<LoweringReplayManifestEntry> entries);

void WriteObjc3FrontendDispatchReplayManifestEntries(
    std::ostream &manifest,
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
    const std::string &control_flow_control_flow_safety_lowering_replay_key,
    const Objc3ControlFlowControlFlowSafetyLoweringContract
        &control_flow_control_flow_safety_lowering_contract,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3SuperDispatchMethodFamilyContract
        &super_dispatch_method_family_contract,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring);

}  // namespace objc3::artifacts::frontend
