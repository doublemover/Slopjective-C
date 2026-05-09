#pragma once

#include <cstdint>

#include "runtime/state/runtime_bootstrap_contracts.h"

namespace objc3c::runtime {

std::uint64_t RuntimeDescriptorTotal(
    const objc3_runtime_image_descriptor *image);
bool RuntimeImageDescriptorHasRequiredIdentity(
    const objc3_runtime_image_descriptor *image);
std::uint64_t RuntimeAggregateCount(
    const objc3_runtime_pointer_aggregate *aggregate);
const void *RuntimeAggregateEntry(
    const objc3_runtime_pointer_aggregate *aggregate, std::uint64_t index);
bool RuntimeAggregateContainsPointer(
    const objc3_runtime_pointer_aggregate *aggregate, const void *target);
bool RuntimeImageDescriptorsMatch(
    const objc3_runtime_image_descriptor *lhs,
    const objc3_runtime_image_descriptor *rhs);
const char *RuntimeImageDescriptorOwnershipModel();

}  // namespace objc3c::runtime
