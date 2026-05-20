#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace objc3c::runtime {

struct RuntimeWeakSlotRef {
  int owner_receiver = 0;
  std::size_t offset = 0;
  std::size_t size = 0;
};

struct RuntimeInstanceRecord {
  std::uint64_t receiver_identity = 0;
  std::uint64_t base_identity = 0;
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t class_receiver_identity = 0;
  std::uint64_t allocation_ordinal = 0;
  std::string class_name;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string instance_isa_owner_identity;
  std::string class_object_isa_owner_identity;
  std::size_t instance_size_bytes = 0;
  std::vector<unsigned char> storage_bytes;
  std::uint64_t retain_count = 1;
};

}  // namespace objc3c::runtime
