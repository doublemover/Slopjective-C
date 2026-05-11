#pragma once

#include "selector_fixtures.h"

#include <cstddef>
#include <cstdint>

namespace objc3c {
namespace runtime {
namespace probe {
namespace selector_lookup_tables {

template <std::size_t EntryCount>
struct PointerAggregateStorage {
  std::uint64_t count;
  const void *entries[EntryCount];
};

static const PointerAggregateStorage<1> kManualEmptyClassRootStorage = {
    0,
    {nullptr},
};
static const PointerAggregateStorage<1> kManualEmptyProtocolRootStorage = {
    0,
    {nullptr},
};
static const PointerAggregateStorage<1> kManualEmptyCategoryRootStorage = {
    0,
    {nullptr},
};
static const PointerAggregateStorage<1> kManualEmptyPropertyRootStorage = {
    0,
    {nullptr},
};
static const PointerAggregateStorage<1> kManualEmptyIvarRootStorage = {
    0,
    {nullptr},
};
static const PointerAggregateStorage<2> kManualSelectorPoolRootStorage = {
    2,
    {kManualSelectorTokenValue, kManualSelectorDebugName},
};
static const PointerAggregateStorage<1> kManualStringPoolRootStorage = {
    1,
    {kManualStringDebugName},
};
static const PointerAggregateStorage<7> kManualDiscoveryRootStorage = {
    7,
    {
        &kManualEmptyClassRootStorage,
        &kManualEmptyProtocolRootStorage,
        &kManualEmptyCategoryRootStorage,
        &kManualEmptyPropertyRootStorage,
        &kManualEmptyIvarRootStorage,
        &kManualSelectorPoolRootStorage,
        &kManualStringPoolRootStorage,
    },
};

static const objc3_runtime_pointer_aggregate *const kManualEmptyClassRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyClassRootStorage);
static const objc3_runtime_pointer_aggregate *const kManualEmptyProtocolRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyProtocolRootStorage);
static const objc3_runtime_pointer_aggregate *const kManualEmptyCategoryRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyCategoryRootStorage);
static const objc3_runtime_pointer_aggregate *const kManualEmptyPropertyRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyPropertyRootStorage);
static const objc3_runtime_pointer_aggregate *const kManualEmptyIvarRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyIvarRootStorage);
static const objc3_runtime_pointer_aggregate *const kManualSelectorPoolRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualSelectorPoolRootStorage);
static const objc3_runtime_pointer_aggregate *const kManualStringPoolRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualStringPoolRootStorage);
static const objc3_runtime_pointer_aggregate *const kManualDiscoveryRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualDiscoveryRootStorage);

static const void *kManualDiscoveryRootAnchorStorage = kManualDiscoveryRoot;
static unsigned char kManualImageLocalInitState = 0;
static const objc3_runtime_registration_table kManualRegistrationTable = {
    2,
    12,
    &kManualImageDescriptor,
    kManualDiscoveryRoot,
    &kManualDiscoveryRootAnchorStorage,
    kManualEmptyClassRoot,
    kManualEmptyProtocolRoot,
    kManualEmptyCategoryRoot,
    kManualEmptyPropertyRoot,
    kManualEmptyIvarRoot,
    kManualSelectorPoolRoot,
    kManualStringPoolRoot,
    nullptr,
    &kManualImageLocalInitState,
};

inline void StageManualSelectorRegistrationTable() {
  objc3_runtime_stage_registration_table_for_bootstrap(&kManualRegistrationTable);
}

inline int RegisterManualSelectorImage() {
  return objc3_runtime_register_image(&kManualImageDescriptor);
}

}  // namespace selector_lookup_tables
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
