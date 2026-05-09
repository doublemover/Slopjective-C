#pragma once

#include <string>

#include "ast/objc3_ast_declarations.h"
#include "lower/contracts/optional_keypath_lowering_contracts.h"
#include "lower/contracts/type_system_generic_lowering_contract_records.h"
#include "runtime/metadata/runtime_metadata_model.h"
#include "sema/objc3_sema_contract_core.h"
#include "sema/objc3_sema_contract_type_handoff.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] Objc3TypeSystemOptionalKeypathLoweringContract
BuildTypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemTypeSemanticModelSummary &summary);

[[nodiscard]] Objc3LightweightGenericsConstraintLoweringContract
BuildLightweightGenericsConstraintLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3NullabilityFlowWarningPrecisionLoweringContract
BuildNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3ProtocolQualifiedObjectTypeLoweringContract
BuildProtocolQualifiedObjectTypeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3VarianceBridgeCastLoweringContract
BuildVarianceBridgeCastLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3GenericMetadataAbiLoweringContract
BuildGenericMetadataAbiLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] std::string BuildTypeSystemGenericContractPreservationJson(
    const Objc3SemanticTypeMetadataHandoff &handoff,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary);

[[nodiscard]] std::string BuildTypeSystemNullabilityContractPreservationJson(
    const Objc3TypeSystemTypeSemanticModelSummary &summary);

[[nodiscard]] std::string BuildTypeSystemProtocolContractPreservationJson(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary);

[[nodiscard]] std::string BuildTypeSystemOptionalKeypathLoweringContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &message_send_selector_lowering_replay_key,
    const std::string &dispatch_abi_marshalling_replay_key,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const std::string &replay_key);

[[nodiscard]] std::string BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary &runtime_link_wiring,
    const std::string &lowering_replay_key);

}  // namespace objc3::artifacts::frontend
