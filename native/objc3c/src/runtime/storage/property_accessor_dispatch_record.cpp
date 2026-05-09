#include "runtime/storage/property_accessor_dispatch_record.h"

#include "runtime/dispatch/builtin_methods.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"

#include <cstring>

namespace objc3c::runtime {

namespace {

bool TryResolveRuntimePropertyGetter(
    const RealizedClassNode &node,
    const RealizedPropertyAccessor &accessor,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution) {
  if (accessor.property_descriptor->effective_getter_selector == nullptr ||
      std::strcmp(accessor.property_descriptor->effective_getter_selector,
                  selector_spelling) != 0) {
    return false;
  }
  if (accessor.getter_return_kind == RuntimeMethodReturnKind::Unsupported) {
    resolution.strict_error_status =
        OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE;
    return true;
  }
  resolution.resolved = true;
  resolution.dispatch_family_is_class = false;
  resolution.selector_storage = selector_spelling;
  resolution.class_name = node.class_name;
  resolution.owner_identity = accessor.getter_owner_identity;
  resolution.normalized_receiver_identity = normalized_receiver_identity;
  resolution.selector_stable_id = selector_stable_id;
  resolution.parameter_count = 0;
  resolution.return_kind = accessor.getter_return_kind;
  resolution.implementation = nullptr;
  resolution.builtin_kind = RuntimeBuiltinKind::PropertyGetter;
  resolution.runtime_property_accessor = &accessor;
  return true;
}

bool TryResolveRuntimePropertySetter(
    const RealizedClassNode &node,
    const RealizedPropertyAccessor &accessor,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution) {
  if (!accessor.property_descriptor->effective_setter_available ||
      accessor.property_descriptor->effective_setter_selector == nullptr ||
      std::strcmp(accessor.property_descriptor->effective_setter_selector,
                  selector_spelling) != 0) {
    return false;
  }
  resolution.resolved = true;
  resolution.dispatch_family_is_class = false;
  resolution.selector_storage = selector_spelling;
  resolution.class_name = node.class_name;
  resolution.owner_identity = accessor.setter_owner_identity;
  resolution.normalized_receiver_identity = normalized_receiver_identity;
  resolution.selector_stable_id = selector_stable_id;
  resolution.parameter_count = 1;
  resolution.return_kind = RuntimeMethodReturnKind::Void;
  resolution.implementation = nullptr;
  resolution.builtin_kind = RuntimeBuiltinKind::PropertySetter;
  resolution.runtime_property_accessor = &accessor;
  return true;
}

}  // namespace

bool TryResolveRuntimePropertyAccessorOnNode(
    const RealizedClassNode &node,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution) {
  if (!node.runtime_layout_ready) {
    return false;
  }
  for (const RealizedPropertyAccessor &accessor :
       node.runtime_property_accessors) {
    if (accessor.property_descriptor == nullptr ||
        accessor.ivar_descriptor == nullptr) {
      continue;
    }
    if (TryResolveRuntimePropertyGetter(node, accessor,
                                        normalized_receiver_identity,
                                        selector_stable_id, selector_spelling,
                                        resolution) ||
        TryResolveRuntimePropertySetter(node, accessor,
                                        normalized_receiver_identity,
                                        selector_stable_id, selector_spelling,
                                        resolution)) {
      return true;
    }
  }
  return false;
}

}  // namespace objc3c::runtime
