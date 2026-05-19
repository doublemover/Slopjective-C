#pragma once

#include <string>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"
#include "lower/contracts/type_system_generic_lowering_contract_records.h"
#include "artifacts/objc3_frontend_type_system_contract_records.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendTypeSystemMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &lightweight_generic_constraint_lowering_replay_key,
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &lightweight_generic_constraint_lowering_contract,
    const std::string &nullability_flow_warning_precision_lowering_replay_key,
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &nullability_flow_warning_precision_lowering_contract,
    const std::string &protocol_qualified_object_type_lowering_replay_key,
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &protocol_qualified_object_type_lowering_contract,
    const std::string &variance_bridge_cast_lowering_replay_key,
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord
        &variance_bridge_cast_lowering_contract,
    const std::string &generic_metadata_abi_lowering_replay_key,
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord
        &generic_metadata_abi_lowering_contract);

}  // namespace objc3::artifacts::frontend
