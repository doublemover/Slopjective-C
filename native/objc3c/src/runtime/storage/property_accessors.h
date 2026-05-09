#pragma once

#include <cstddef>

namespace objc3c::runtime {

struct RealizedClassNode;
struct RealizedPropertyAccessor;
struct RuntimeInstanceRecord;
struct RuntimeState;

bool RuntimePropertyAccessorSelectorIsMaterializable(const char *selector);
bool RuntimePropertySetterHasSupportedArity(unsigned long long parameter_count);
std::size_t EffectiveIvarOffset(const RealizedPropertyAccessor &accessor);
std::size_t EffectiveIvarSize(const RealizedPropertyAccessor &accessor);
bool UsesStrongOwnedRuntimeHooks(const RealizedPropertyAccessor &accessor);
bool UsesWeakRuntimeHooks(const RealizedPropertyAccessor &accessor);
bool UsesSafeUnownedRuntimeHooks(const RealizedPropertyAccessor &accessor);
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
const RealizedPropertyAccessor *FindRuntimePropertyAccessorByNameUnlocked(
    RuntimeState &state,
    const RealizedClassNode &start_node,
    const char *property_name,
    const RealizedClassNode *&resolved_node,
    bool &inherited,
    bool &used_cache);

}  // namespace objc3c::runtime
