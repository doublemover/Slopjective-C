#pragma once

#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

#include <string>

// Runtime metadata layout policy readiness contracts own the fail-closed build
// gate that turns upstream metadata evidence into an emission-ready policy.
bool TryBuildObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error);
bool IsReadyObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicy &policy);
