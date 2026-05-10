#pragma once

#include <cstddef>
#include <string>

#include "artifacts/objc3_frontend_artifact_dispatch_contract_types.h"
#include "pipeline/objc3_frontend_types.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildDispatchDispatchIntentSemanticModelSummaryJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &summary);

[[nodiscard]] std::string BuildDispatchDispatchIntentLegalitySummaryJson(
    const Objc3DispatchDispatchIntentLegalitySummary &summary);

[[nodiscard]] std::string BuildDispatchDispatchIntentCompatibilitySummaryJson(
    const Objc3DispatchDispatchIntentCompatibilitySummary &summary);

[[nodiscard]] std::string BuildDispatchDispatchControlLoweringContractJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary &compatibility_summary,
    const Objc3DispatchDispatchControlLoweringContract &contract,
    const std::string &replay_key);

[[nodiscard]] Objc3DispatchSurfaceClassificationContract
BuildDispatchSurfaceClassificationContract(const Objc3Program &program);

[[nodiscard]] Objc3IdClassSelObjectPointerTypecheckContract
BuildIdClassSelObjectPointerTypecheckContract(const Objc3Program &program);

[[nodiscard]] Objc3PropertySynthesisIvarBindingContract
BuildPropertySynthesisIvarBindingContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3MessageSendSelectorLoweringContract
BuildMessageSendSelectorLoweringContract(const Objc3Program &program);

[[nodiscard]] Objc3DispatchAbiMarshallingContract
BuildDispatchAbiMarshallingContract(const Objc3Program &program,
                                    std::size_t runtime_dispatch_arg_slots);

[[nodiscard]] Objc3NilReceiverSemanticsFoldabilityContract
BuildNilReceiverSemanticsFoldabilityContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3SuperDispatchMethodFamilyContract
BuildSuperDispatchMethodFamilyContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3RuntimeLinkHostLinkContract BuildRuntimeLinkHostLinkContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3NilReceiverSemanticsFoldabilityContract
        &nil_receiver_semantics_foldability_contract,
    const Objc3FrontendOptions &options);

[[nodiscard]] Objc3RuntimeDispatchLoweringAbiContract
BuildRuntimeDispatchLoweringAbiContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api_summary);

}  // namespace objc3::artifacts::frontend
