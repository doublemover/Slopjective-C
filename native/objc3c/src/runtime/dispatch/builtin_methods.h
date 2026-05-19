#pragma once

#include <cstdint>

namespace objc3c::runtime {

enum class RuntimeBuiltinKind {
  None = 0,
  Alloc = 1,
  Init = 2,
  New = 3,
  PropertyGetter = 4,
  PropertySetter = 5,
};

struct RealizedPropertyAccessor;
struct RuntimeState;
struct RuntimeTypedDispatchResult;

const char *DescribeResolvedImplementationKind(
    RuntimeBuiltinKind builtin_kind, const void *implementation);
RuntimeTypedDispatchResult InvokeRuntimeBuiltinMethod(
    RuntimeState &state, RuntimeBuiltinKind builtin_kind, int receiver,
    std::uint64_t base_identity,
    const RealizedPropertyAccessor *runtime_property_accessor, int a0, int a1,
    int a2, int a3);

}  // namespace objc3c::runtime
