#pragma once

namespace objc3c::runtime {

struct RealizedPropertyAccessor;

bool UsesStrongOwnedRuntimeHooks(const RealizedPropertyAccessor &accessor);
bool UsesWeakRuntimeHooks(const RealizedPropertyAccessor &accessor);
bool UsesSafeUnownedRuntimeHooks(const RealizedPropertyAccessor &accessor);

}  // namespace objc3c::runtime
