#pragma once

#include "lower/objc3_lowering_contract.h"

#include <array>

extern const std::array<const char *,
                        kObjc3RuntimeMetadataLayoutPolicyFamilyCount>
    kCanonicalRuntimeMetadataFamilyOrder;
