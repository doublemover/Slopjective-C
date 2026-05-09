#pragma once

#include <string>

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/type_system_generic_lowering_contract_records.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendTypeSystemMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &lightweight_generic_constraint_lowering_replay_key,
    const Objc3LightweightGenericsConstraintLoweringContract
        &lightweight_generic_constraint_lowering_contract,
    const std::string &nullability_flow_warning_precision_lowering_replay_key,
    const Objc3NullabilityFlowWarningPrecisionLoweringContract
        &nullability_flow_warning_precision_lowering_contract,
    const std::string &protocol_qualified_object_type_lowering_replay_key,
    const Objc3ProtocolQualifiedObjectTypeLoweringContract
        &protocol_qualified_object_type_lowering_contract,
    const std::string &variance_bridge_cast_lowering_replay_key,
    const Objc3VarianceBridgeCastLoweringContract
        &variance_bridge_cast_lowering_contract,
    const std::string &generic_metadata_abi_lowering_replay_key,
    const Objc3GenericMetadataAbiLoweringContract
        &generic_metadata_abi_lowering_contract);

}  // namespace objc3::artifacts::frontend
