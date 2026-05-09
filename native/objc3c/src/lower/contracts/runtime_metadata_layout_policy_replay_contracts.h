#pragma once

#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

#include <string>

// Runtime metadata layout policy replay contracts own the deterministic key
// surface used to compare policy evidence across lowerer and artifact stages.
std::string Objc3RuntimeMetadataLayoutPolicyReplayKey(
    const Objc3RuntimeMetadataLayoutPolicy &policy);
