#pragma once

#include <iosfwd>
#include <string>

#include "artifacts/objc3_frontend_type_system_contract_records.h"

namespace objc3::artifacts::frontend {

void WriteTypeSystemManifestSurfaces(
    std::ostream &manifest,
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &lightweight_generic_constraint_lowering_contract,
    const std::string &lightweight_generic_constraint_lowering_replay_key,
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &nullability_flow_warning_precision_lowering_contract,
    const std::string &nullability_flow_warning_precision_lowering_replay_key,
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &protocol_qualified_object_type_lowering_contract,
    const std::string &protocol_qualified_object_type_lowering_replay_key,
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord
        &variance_bridge_cast_lowering_contract,
    const std::string &variance_bridge_cast_lowering_replay_key,
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord
        &generic_metadata_abi_lowering_contract,
    const std::string &generic_metadata_abi_lowering_replay_key);

}  // namespace objc3::artifacts::frontend
