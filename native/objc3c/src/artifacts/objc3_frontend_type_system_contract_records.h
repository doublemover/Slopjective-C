#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "lower/contracts/optional_keypath_lowering_contracts.h"
#include "lower/contracts/runtime_metadata_source_record_contracts.h"
#include "lower/contracts/type_system_generic_lowering_contracts.h"
#include "sema/model/frontend_type_source_closure.h"

namespace objc3::artifacts::frontend {

inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringContractId =
        ::kObjc3TypeSystemOptionalKeypathLoweringContractId;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_lowering_contract";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringOptionalModel =
        ::kObjc3TypeSystemOptionalKeypathLoweringOptionalModel;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringTypedKeypathModel =
        ::kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringAuthorityModel =
        ::kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringFailClosedModel =
        ::kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperContractId =
        ::kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperSurfacePath =
        ::kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperOptionalModel =
        ::kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel =
        ::kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperDiagnosticModel =
        ::kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel;
inline constexpr const char
    *kObjc3FrontendTypeSystemRuntimeKeypathDescriptorLogicalSection =
        ::kObjc3RuntimeKeypathDescriptorLogicalSection;

inline constexpr const char
    *kObjc3FrontendTypeSystemTypeSemanticModelContractId =
        ::kObjc3TypeSystemTypeSemanticModelContractId;
inline constexpr const char
    *kObjc3FrontendTypeSystemGenericContractPreservationContractId =
        ::kObjc3TypeSystemGenericContractPreservationContractId;
inline constexpr const char
    *kObjc3FrontendTypeSystemNullabilityContractPreservationContractId =
        ::kObjc3TypeSystemNullabilityContractPreservationContractId;
inline constexpr const char
    *kObjc3FrontendTypeSystemProtocolContractPreservationContractId =
        ::kObjc3TypeSystemProtocolContractPreservationContractId;
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringLaneContract =
        ::kObjc3TypeSystemOptionalKeypathLoweringLaneContract;
inline constexpr const char
    *kObjc3FrontendLightweightGenericsConstraintLoweringLaneContract =
        ::kObjc3LightweightGenericsConstraintLoweringLaneContract;
inline constexpr const char
    *kObjc3FrontendNullabilityFlowWarningPrecisionLoweringLaneContract =
        ::kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract;
inline constexpr const char
    *kObjc3FrontendProtocolQualifiedObjectTypeLoweringLaneContract =
        ::kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract;
inline constexpr const char
    *kObjc3FrontendVarianceBridgeCastLoweringLaneContract =
        ::kObjc3VarianceBridgeCastLoweringLaneContract;
inline constexpr const char
    *kObjc3FrontendGenericMetadataAbiLoweringLaneContract =
        ::kObjc3GenericMetadataAbiLoweringLaneContract;

using Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord =
    ::Objc3TypeSystemOptionalKeypathLoweringContract;
using Objc3FrontendLightweightGenericsConstraintLoweringContractRecord =
    ::Objc3LightweightGenericsConstraintLoweringContract;
using Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord =
    ::Objc3NullabilityFlowWarningPrecisionLoweringContract;
using Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord =
    ::Objc3ProtocolQualifiedObjectTypeLoweringContract;
using Objc3FrontendVarianceBridgeCastLoweringContractRecord =
    ::Objc3VarianceBridgeCastLoweringContract;
using Objc3FrontendGenericMetadataAbiLoweringContractRecord =
    ::Objc3GenericMetadataAbiLoweringContract;

struct Objc3FrontendTypeSystemSemanticModelRecord {
  std::string contract_id = kObjc3FrontendTypeSystemTypeSemanticModelContractId;
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t canonical_type_entries = 0;
  std::size_t canonical_object_type_entries = 0;
  std::size_t canonical_nullable_entries = 0;
  std::size_t canonical_nonnull_entries = 0;
  std::size_t canonical_implicitly_unwrapped_entries = 0;
  std::size_t canonical_null_resettable_entries = 0;
  std::size_t canonical_unspecified_nullability_entries = 0;
  std::size_t canonical_invalid_type_entries = 0;
  std::size_t optional_binding_contract_violation_sites = 0;
  std::size_t optional_send_contract_violation_sites = 0;
  std::size_t optional_flow_contract_violation_sites = 0;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
};

#include "artifacts/objc3_frontend_type_system_contract_records_parity_surface.inc"

#include "artifacts/objc3_frontend_type_system_contract_records_semantic_metadata.inc"

#include "artifacts/objc3_frontend_type_system_contract_records_inventories.inc"

[[nodiscard]] bool IsValidFrontendTypeSystemOptionalKeypathLoweringContract(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &contract);
[[nodiscard]] std::string FrontendTypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &contract);
[[nodiscard]] bool
IsValidFrontendLightweightGenericsConstraintLoweringContract(
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &contract);
[[nodiscard]] std::string
FrontendLightweightGenericsConstraintLoweringReplayKey(
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &contract);
[[nodiscard]] bool
IsValidFrontendNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &contract);
[[nodiscard]] std::string
FrontendNullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &contract);
[[nodiscard]] bool
IsValidFrontendProtocolQualifiedObjectTypeLoweringContract(
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &contract);
[[nodiscard]] std::string
FrontendProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &contract);
[[nodiscard]] bool IsValidFrontendVarianceBridgeCastLoweringContract(
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord &contract);
[[nodiscard]] std::string FrontendVarianceBridgeCastLoweringReplayKey(
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord &contract);
[[nodiscard]] bool IsValidFrontendGenericMetadataAbiLoweringContract(
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord &contract);
[[nodiscard]] std::string FrontendGenericMetadataAbiLoweringReplayKey(
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord &contract);

}  // namespace objc3::artifacts::frontend
