#pragma once

#include "lower/objc3_lowering_contract.h"

#include <array>
#include <cstddef>

std::size_t CountRuntimeMetadataLayoutDescriptors(
    const std::array<Objc3RuntimeMetadataLayoutPolicyFamily,
                     kObjc3RuntimeMetadataLayoutPolicyFamilyCount> &families);
