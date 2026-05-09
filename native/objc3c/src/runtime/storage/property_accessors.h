#pragma once

#include <cstddef>

#include "runtime/storage/property_accessor_dispatch_record.h"
#include "runtime/storage/property_accessor_profiles.h"
#include "runtime/storage/property_accessor_records.h"
#include "runtime/storage/property_accessor_resolution.h"
#include "runtime/storage/property_layout_realization.h"
#include "runtime/storage/property_lookup.h"
#include "runtime/storage/property_value_storage.h"

namespace objc3c::runtime {

struct RealizedClassNode;
struct RealizedPropertyAccessor;
struct RuntimeInstanceRecord;
struct RuntimeState;

bool RuntimePropertyAccessorSelectorIsMaterializable(const char *selector);
bool RuntimePropertySetterHasSupportedArity(unsigned long long parameter_count);

}  // namespace objc3c::runtime
