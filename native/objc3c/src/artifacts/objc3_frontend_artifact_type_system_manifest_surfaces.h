#pragma once

#include <iosfwd>
#include <string>

struct Objc3GenericMetadataAbiLoweringContract;
struct Objc3LightweightGenericsConstraintLoweringContract;
struct Objc3NullabilityFlowWarningPrecisionLoweringContract;
struct Objc3ProtocolQualifiedObjectTypeLoweringContract;
struct Objc3VarianceBridgeCastLoweringContract;

namespace objc3::artifacts::frontend {

void WriteTypeSystemManifestSurfaces(
    std::ostream &manifest,
    const Objc3LightweightGenericsConstraintLoweringContract
        &lightweight_generic_constraint_lowering_contract,
    const std::string &lightweight_generic_constraint_lowering_replay_key,
    const Objc3NullabilityFlowWarningPrecisionLoweringContract
        &nullability_flow_warning_precision_lowering_contract,
    const std::string &nullability_flow_warning_precision_lowering_replay_key,
    const Objc3ProtocolQualifiedObjectTypeLoweringContract
        &protocol_qualified_object_type_lowering_contract,
    const std::string &protocol_qualified_object_type_lowering_replay_key,
    const Objc3VarianceBridgeCastLoweringContract
        &variance_bridge_cast_lowering_contract,
    const std::string &variance_bridge_cast_lowering_replay_key,
    const Objc3GenericMetadataAbiLoweringContract
        &generic_metadata_abi_lowering_contract,
    const std::string &generic_metadata_abi_lowering_replay_key);

}  // namespace objc3::artifacts::frontend
