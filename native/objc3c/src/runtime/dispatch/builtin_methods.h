#pragma once

namespace objc3c::runtime {

enum class RuntimeBuiltinKind {
  None = 0,
  Alloc = 1,
  Init = 2,
  New = 3,
  PropertyGetter = 4,
  PropertySetter = 5,
};

const char *DescribeResolvedImplementationKind(
    RuntimeBuiltinKind builtin_kind, const void *implementation);

}  // namespace objc3c::runtime
