#pragma once

#include "runtime/storage/ivar_storage_span.h"

namespace objc3c::runtime {

struct RealizedPropertyAccessor;
struct RuntimeInstanceRecord;
struct RuntimeState;

bool ReadRuntimeManagedPropertyValueRaw(
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int &value);
bool WriteRuntimeManagedPropertyValueRaw(
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value);
bool ReadRuntimeManagedPropertyValueUnlocked(
    const RuntimeState &state,
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int &value);
bool WriteRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value);
bool ExchangeRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value,
    int &previous_value);

}  // namespace objc3c::runtime
