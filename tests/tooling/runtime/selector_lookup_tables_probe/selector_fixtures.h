#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace selector_lookup_tables {

static constexpr char kManualModuleName[] = "dispatch_runtime-manual-image";
static constexpr char kManualIdentityKey[] = "dispatch_runtime::manual-image";
static constexpr char kManualSelectorTokenValue[] = "tokenValue";
static constexpr char kManualSelectorDebugName[] = "debugName";
static constexpr char kManualStringDebugName[] = "debugName";
static constexpr char kDynamicSelector[] = "manualOnly:";
static constexpr char kCurrentValueSelector[] = "currentValue";
static constexpr char kSetCurrentValueSelector[] = "setCurrentValue:";
static constexpr char kSharedSelector[] = "shared";

static const objc3_runtime_image_descriptor kManualImageDescriptor{
    kManualModuleName,
    kManualIdentityKey,
    2,
    0,
    0,
    0,
    0,
    0,
};

}  // namespace selector_lookup_tables
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
