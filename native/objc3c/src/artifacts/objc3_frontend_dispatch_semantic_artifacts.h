#pragma once

#include <cstddef>
#include <string>

#include "lower/contracts/dispatch_abi_marshalling_contracts.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "lower/contracts/dispatch_surface_classification_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/nil_receiver_semantics_foldability_contracts.h"
#include "lower/contracts/object_model_lowering_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"
#include "lower/contracts/super_dispatch_method_family_contracts.h"
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
