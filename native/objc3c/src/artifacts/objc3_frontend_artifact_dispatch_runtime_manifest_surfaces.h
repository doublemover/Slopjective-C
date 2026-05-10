#pragma once

#include <iosfwd>
#include <string>

struct Objc3DispatchAbiMarshallingContract;
struct Objc3DispatchSurfaceClassificationContract;
struct Objc3MessageSendSelectorLoweringContract;
struct Objc3NilReceiverSemanticsFoldabilityContract;
struct Objc3RuntimeDispatchLoweringAbiContract;
struct Objc3RuntimeLinkHostLinkContract;
struct Objc3SuperDispatchMethodFamilyContract;

namespace objc3::artifacts::frontend {

void WriteDispatchRuntimeAbiManifestSurfaces(
    std::ostream &manifest,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const std::string &dispatch_surface_classification_replay_key,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract,
    const std::string &message_send_selector_lowering_replay_key,
    const Objc3DispatchAbiMarshallingContract
        &dispatch_abi_marshalling_contract,
    const std::string &dispatch_abi_marshalling_replay_key,
    const Objc3NilReceiverSemanticsFoldabilityContract
        &nil_receiver_semantics_foldability_contract,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const Objc3SuperDispatchMethodFamilyContract
        &super_dispatch_method_family_contract,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeDispatchLoweringAbiContract
        &runtime_dispatch_lowering_abi_contract,
    const std::string &runtime_dispatch_lowering_abi_replay_key);

}  // namespace objc3::artifacts::frontend
