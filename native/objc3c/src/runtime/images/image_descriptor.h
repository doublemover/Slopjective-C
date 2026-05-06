#pragma once

#include <cstdint>

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

std::uint64_t RuntimeDescriptorTotal(
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

}  // namespace objc3c::runtime
