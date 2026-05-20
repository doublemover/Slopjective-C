#pragma once

#include "runtime/dispatch/builtin_methods.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/public/objc3_runtime_result.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

namespace objc3c::runtime {

struct PropertyLookupCacheKey {
  std::uint64_t start_base_identity = 0;
  std::string property_name;

  bool operator==(const PropertyLookupCacheKey &other) const {
    return start_base_identity == other.start_base_identity &&
           property_name == other.property_name;
  }
};

struct PropertyLookupCacheKeyHash {
  std::size_t operator()(const PropertyLookupCacheKey &key) const {
    return std::hash<std::uint64_t>{}(key.start_base_identity) ^
           (std::hash<std::string>{}(key.property_name) << 1u);
  }
};

struct PropertyLookupCacheEntry {
  bool found = false;
  bool inherited = false;
  std::size_t resolved_node_index = 0;
  std::size_t accessor_index = 0;
  std::uint64_t cache_class_graph_generation = 0;
  std::uint64_t cache_category_attachment_generation = 0;
  std::uint64_t cache_storage_surface_generation = 0;
};

struct MethodCacheEntry {
  bool resolved = false;
  bool dispatch_family_is_class = false;
  bool fast_path_seeded = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::string selector_storage;
  std::string fast_path_reason;
  std::string class_name;
  std::string owner_identity;
  std::uint64_t lookup_start_base_identity = 0;
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;
  std::uint64_t parameter_count = 0;
  RuntimeMethodReturnKind return_kind = RuntimeMethodReturnKind::Unsupported;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;
  std::uint64_t cache_registered_image_count = 0;
  std::uint64_t cache_last_successful_registration_order_ordinal = 0;
  std::uint64_t cache_reset_generation = 0;
  std::uint64_t cache_replay_generation = 0;
  std::uint64_t cache_realized_class_node_count = 0;
  std::uint64_t cache_class_graph_generation = 0;
  std::uint64_t cache_category_attachment_generation = 0;
  std::uint64_t cache_protocol_declaration_generation = 0;
  std::uint64_t cache_storage_surface_generation = 0;
  std::uint64_t cache_method_surface_generation = 0;
  objc3_runtime_dispatch_status_code strict_error_status =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
  const void *implementation = nullptr;
  RuntimeBuiltinKind builtin_kind = RuntimeBuiltinKind::None;
  const RealizedPropertyAccessor *runtime_property_accessor = nullptr;
};

struct SlowPathResolution {
  bool resolved = false;
  bool ambiguous = false;
  bool dispatch_family_is_class = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::string selector_storage;
  std::string fast_path_reason;
  std::string class_name;
  std::string owner_identity;
  std::uint64_t lookup_start_base_identity = 0;
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;
  std::uint64_t parameter_count = 0;
  RuntimeMethodReturnKind return_kind = RuntimeMethodReturnKind::Unsupported;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;
  objc3_runtime_dispatch_status_code strict_error_status =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
  const void *implementation = nullptr;
  RuntimeBuiltinKind builtin_kind = RuntimeBuiltinKind::None;
  const RealizedPropertyAccessor *runtime_property_accessor = nullptr;
};

inline bool HasTerminalStrictDispatchError(
    const SlowPathResolution &resolution) {
  return !resolution.resolved &&
         resolution.strict_error_status !=
             OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
}

}  // namespace objc3c::runtime
