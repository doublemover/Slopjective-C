#pragma once

#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace objc3c::runtime {

struct RealizedPropertyAccessor {
  const EmittedPropertyDescriptor *property_descriptor = nullptr;
  const EmittedIvarDescriptor *ivar_descriptor = nullptr;
  RuntimeMethodReturnKind getter_return_kind =
      RuntimeMethodReturnKind::Unsupported;
  std::string getter_owner_identity;
  std::string setter_owner_identity;
};

struct RealizedClassNode {
  std::string module_name;
  std::string translation_unit_identity_key;
  std::string class_name;
  std::string bundle_owner_identity;
  std::string interface_owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::uint64_t registration_order_ordinal = 0;
  std::uint64_t base_identity = 0;
  bool is_root_class = false;
  bool implementation_backed = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  bool runtime_attachment_ready = false;
  bool runtime_layout_ready = false;
  std::size_t runtime_instance_size_bytes = 0;
  const RegisteredImageMetadata *image = nullptr;
  const EmittedClassBundle *bundle = nullptr;
  std::vector<const EmittedCategoryRecord *> attached_category_records;
  std::vector<RealizedPropertyAccessor> runtime_property_accessors;
  std::size_t super_node_index = 0;
  bool has_super_node = false;
};

}  // namespace objc3c::runtime
