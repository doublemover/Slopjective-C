#pragma once

#include "lower/contracts/type_system_generic_lowering_contract_records.h"

#include <string>

bool IsValidObjc3LightweightGenericsConstraintLoweringContract(
    const Objc3LightweightGenericsConstraintLoweringContract &contract);
std::string Objc3LightweightGenericsConstraintLoweringReplayKey(
    const Objc3LightweightGenericsConstraintLoweringContract &contract);
bool IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract);
std::string Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract);
bool IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract);
std::string Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract);
bool IsValidObjc3VarianceBridgeCastLoweringContract(
    const Objc3VarianceBridgeCastLoweringContract &contract);
std::string Objc3VarianceBridgeCastLoweringReplayKey(
    const Objc3VarianceBridgeCastLoweringContract &contract);
bool IsValidObjc3GenericMetadataAbiLoweringContract(
    const Objc3GenericMetadataAbiLoweringContract &contract);
std::string Objc3GenericMetadataAbiLoweringReplayKey(
    const Objc3GenericMetadataAbiLoweringContract &contract);
