#pragma once

#include "runtime/dispatch/runtime_method_return.h"

#include <cstddef>
#include <string>

namespace objc3c::runtime {

struct EmittedIvarDescriptor;
struct EmittedPropertyDescriptor;
struct RealizedPropertyAccessor;

std::size_t AlignRuntimePropertyStorageSize(std::size_t value,
                                            std::size_t alignment);
bool RuntimePropertyIvarDescriptorHasStrictPublishedLayout(
    const EmittedIvarDescriptor &descriptor,
    std::size_t effective_offset,
    std::size_t effective_alignment);
bool RuntimePropertyIvarStorageExtentIsAddressable(std::size_t offset,
                                                   std::size_t size);
std::string BuildRuntimePropertyAccessorOwnerIdentity(
    const char *declaration_owner_identity,
    const char *selector);
RuntimeMethodReturnKind ClassifyRuntimePropertyAccessorReturnType(
    const EmittedPropertyDescriptor &descriptor);
bool RuntimePropertyAccessorSortsBefore(
    const RealizedPropertyAccessor &lhs,
    const RealizedPropertyAccessor &rhs);

}  // namespace objc3c::runtime
