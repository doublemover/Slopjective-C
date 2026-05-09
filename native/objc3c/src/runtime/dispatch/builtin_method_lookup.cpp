#include "runtime/dispatch/builtin_method_lookup.h"

#include "runtime/dispatch/builtin_methods.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/runtime_resolution_records.h"

#include <string>

namespace objc3c::runtime {

bool TryResolveRuntimeBuiltinObjectMethod(
    const std::string &class_name,
    DispatchFamily family,
    const char *selector_spelling,
    SlowPathResolution &resolution) {
  if (class_name.empty() || selector_spelling == nullptr ||
      selector_spelling[0] == '\0') {
    return false;
  }
  const std::string selector = selector_spelling;
  // instance-allocation-layout-runtime anchor: builtin alloc/new/init remain
  // runtime-owned lookup results backed by realized class layout.
  if (family == DispatchFamily::Class &&
      (selector == "alloc" || selector == "new")) {
    resolution.resolved = true;
    resolution.dispatch_family_is_class = true;
    resolution.class_name = class_name;
    resolution.selector_storage = selector;
    resolution.owner_identity =
        "runtime-builtin:" + class_name + "::class_method:" + selector;
    resolution.parameter_count = 0;
    resolution.return_kind = RuntimeMethodReturnKind::ObjectReference;
    resolution.implementation = nullptr;
    resolution.builtin_kind =
        selector == "alloc" ? RuntimeBuiltinKind::Alloc : RuntimeBuiltinKind::New;
    return true;
  }
  if (family == DispatchFamily::Instance && selector == "init") {
    resolution.resolved = true;
    resolution.dispatch_family_is_class = false;
    resolution.class_name = class_name;
    resolution.selector_storage = selector;
    resolution.owner_identity =
        "runtime-builtin:" + class_name + "::instance_method:init";
    resolution.parameter_count = 0;
    resolution.return_kind = RuntimeMethodReturnKind::ObjectReference;
    resolution.implementation = nullptr;
    resolution.builtin_kind = RuntimeBuiltinKind::Init;
    return true;
  }
  return false;
}

}  // namespace objc3c::runtime
