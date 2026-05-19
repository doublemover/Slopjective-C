#pragma once

#include "runtime/dispatch/builtin_methods.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/public/objc3_runtime_result.h"

#include <cstdint>

namespace objc3c::runtime {

struct RealizedPropertyAccessor;
struct RuntimeState;

struct RuntimeDispatchTarget {
  const void *implementation = nullptr;
  const RealizedPropertyAccessor *runtime_property_accessor = nullptr;
  std::uint64_t parameter_count = 0;
  RuntimeMethodReturnKind return_kind = RuntimeMethodReturnKind::Unsupported;
  RuntimeBuiltinKind builtin_kind = RuntimeBuiltinKind::None;
  bool resolved_live_method = false;
  std::uint64_t receiver_base_identity = 0;
  objc3_runtime_dispatch_status_code dispatch_status =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
};

RuntimeDispatchTarget ResolveRuntimeDispatchTargetUnlocked(
    RuntimeState &state, int receiver, const char *selector);

}  // namespace objc3c::runtime
