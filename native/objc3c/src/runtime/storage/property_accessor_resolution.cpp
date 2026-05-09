#include "runtime/storage/property_accessor_resolution.h"

#include "runtime/dispatch/builtin_methods.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"

#include <cstring>
#include <unordered_set>

namespace objc3c::runtime {

bool TryResolveRuntimeManagedPropertyAccessorUnlocked(
    const RuntimeState &state, const RealizedClassNode &start_node,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    SlowPathResolution &resolution) {
  if (selector_spelling == nullptr || selector_spelling[0] == '\0') {
    return false;
  }
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = &start_node;
  while (node != nullptr && visited.insert(node).second) {
    if (node->runtime_layout_ready) {
      for (const RealizedPropertyAccessor &accessor :
           node->runtime_property_accessors) {
        if (accessor.property_descriptor == nullptr ||
            accessor.ivar_descriptor == nullptr) {
          continue;
        }
        if (accessor.property_descriptor->effective_getter_selector != nullptr &&
            std::strcmp(accessor.property_descriptor->effective_getter_selector,
                        selector_spelling) == 0) {
          if (accessor.getter_return_kind ==
              RuntimeMethodReturnKind::Unsupported) {
            resolution.strict_error_status =
                OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE;
            return true;
          }
          resolution.resolved = true;
          resolution.dispatch_family_is_class = false;
          resolution.selector_storage = selector_spelling;
          resolution.class_name = node->class_name;
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
        if (accessor.property_descriptor->effective_setter_available &&
            accessor.property_descriptor->effective_setter_selector != nullptr &&
            std::strcmp(accessor.property_descriptor->effective_setter_selector,
                        selector_spelling) == 0) {
          resolution.resolved = true;
          resolution.dispatch_family_is_class = false;
          resolution.selector_storage = selector_spelling;
          resolution.class_name = node->class_name;
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
      }
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  return false;
}

}  // namespace objc3c::runtime
