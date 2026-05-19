#pragma once

#include <iosfwd>
#include <string>

#include "artifacts/objc3_frontend_artifact_dispatch_contract_snapshots.h"

namespace objc3::artifacts::frontend {

void WriteDispatchRuntimeAbiManifestSurfaces(
    std::ostream &manifest,
    const Objc3DispatchSurfaceClassificationSnapshot
        &dispatch_surface_classification,
    const std::string &dispatch_surface_classification_replay_key,
    const Objc3MessageSendSelectorLoweringSnapshot
        &message_send_selector_lowering,
    const std::string &message_send_selector_lowering_replay_key,
    const Objc3DispatchAbiMarshallingSnapshot &dispatch_abi_marshalling,
    const std::string &dispatch_abi_marshalling_replay_key,
    const Objc3NilReceiverSemanticsFoldabilitySnapshot
        &nil_receiver_semantics_foldability,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const Objc3SuperDispatchMethodFamilySnapshot &super_dispatch_method_family,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3RuntimeLinkHostLinkSnapshot &runtime_link_host_link,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeDispatchLoweringAbiSnapshot
        &runtime_dispatch_lowering_abi,
    const std::string &runtime_dispatch_lowering_abi_replay_key);

}  // namespace objc3::artifacts::frontend
