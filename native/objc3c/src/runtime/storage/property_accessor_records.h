#pragma once

#include "runtime/dispatch/runtime_method_return.h"

#include <string>

namespace objc3c::runtime {

struct EmittedIvarDescriptor;
struct EmittedPropertyDescriptor;
struct RealizedPropertyAccessor;

bool RuntimePropertyDescriptorHasRealizableAccessorShape(
    const EmittedPropertyDescriptor &descriptor);
bool RuntimePropertyDescriptorDeclaresSynthesizedStorageBinding(
    const EmittedPropertyDescriptor &descriptor);
bool BuildRuntimePropertyAccessorRecord(
    const EmittedPropertyDescriptor &descriptor,
    const EmittedIvarDescriptor &ivar_descriptor,
    RealizedPropertyAccessor &accessor);
std::string BuildRuntimePropertyAccessorOwnerIdentity(
    const char *declaration_owner_identity,
    const char *selector);
RuntimeMethodReturnKind ClassifyRuntimePropertyAccessorReturnType(
    const EmittedPropertyDescriptor &descriptor);
bool RuntimePropertyAccessorSortsBefore(
    const RealizedPropertyAccessor &lhs,
    const RealizedPropertyAccessor &rhs);

}  // namespace objc3c::runtime
