#pragma once

#include <string>

// Runtime metadata gates own the summary evidence boundary and object-emission
// closeout contract after metadata publication/source-record emission.
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateContractId =
    "objc3c.runtime.metadata.emission.gate.v1";
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateEvidenceModel =
    "source-sema-ir-runtime-summary-chain";
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateFailureModel =
    "fail-closed-on-upstream-summary-drift";
inline constexpr const char
    *kObjc3RuntimeMetadataObjectEmissionCloseoutContractId =
        "objc3c.runtime.cross.lane.object.emission.closeout.v1";
inline constexpr const char
    *kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel =
        "integrated-summary-plus-native-object-emission-probes";
inline constexpr const char
    *kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel =
        "fail-closed-on-summary-or-integrated-probe-drift";

std::string Objc3RuntimeMetadataEmissionGateSummary();
std::string Objc3RuntimeMetadataObjectEmissionCloseoutSummary();
