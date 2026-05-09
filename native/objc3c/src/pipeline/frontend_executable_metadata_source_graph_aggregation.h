#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace objc3c::pipeline::orchestration {

struct ExecutableMetadataAggregatedClassSurface {
  bool has_interface = false;
  bool has_implementation = false;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string super_class_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  bool objc_direct_members_declared = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::size_t interface_property_count = 0;
  std::size_t implementation_property_count = 0;
  std::size_t interface_method_count = 0;
  std::size_t implementation_method_count = 0;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct ExecutableMetadataAggregatedCategorySurface {
  bool has_interface = false;
  bool has_implementation = false;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string class_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  std::size_t interface_property_count = 0;
  std::size_t implementation_property_count = 0;
  std::size_t interface_method_count = 0;
  std::size_t implementation_method_count = 0;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct ExecutableMetadataMethodEdgeRecord {
  std::string owner_identity;
  std::string export_owner_identity;
  std::string selector;
  bool is_class_method = false;
};

}  // namespace objc3c::pipeline::orchestration
