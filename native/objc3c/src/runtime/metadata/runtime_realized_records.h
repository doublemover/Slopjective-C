#pragma once

#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/metadata/runtime_emitted_records.h"

#include <string>

namespace objc3c::runtime {

struct RealizedPropertyAccessor {
  const EmittedPropertyDescriptor *property_descriptor = nullptr;
  const EmittedIvarDescriptor *ivar_descriptor = nullptr;
  RuntimeMethodReturnKind getter_return_kind =
      RuntimeMethodReturnKind::Unsupported;
  std::string getter_owner_identity;
  std::string setter_owner_identity;
};

}  // namespace objc3c::runtime
