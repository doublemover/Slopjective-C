#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_STORAGE_FIXTURE_DEFINITIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_STORAGE_FIXTURE_DEFINITIONS_H_

namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection {

struct StorageOwnershipPropertyQuery {
  const char *class_name;
  const char *property_name;
};

inline constexpr const char *kBoxClassName = "Box";

inline constexpr StorageOwnershipPropertyQuery kCurrentValuePropertyQuery{
    kBoxClassName, "currentValue"};
inline constexpr StorageOwnershipPropertyQuery kCopiedValuePropertyQuery{
    kBoxClassName, "copiedValue"};
inline constexpr StorageOwnershipPropertyQuery kWeakValuePropertyQuery{
    kBoxClassName, "weakValue"};
inline constexpr StorageOwnershipPropertyQuery kBorrowedValuePropertyQuery{
    kBoxClassName, "borrowedValue"};
inline constexpr StorageOwnershipPropertyQuery kGuardedValuePropertyQuery{
    kBoxClassName, "guardedValue"};

}  // namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_STORAGE_FIXTURE_DEFINITIONS_H_
