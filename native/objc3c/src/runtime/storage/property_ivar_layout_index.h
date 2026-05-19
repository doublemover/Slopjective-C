#pragma once

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace objc3c::runtime {

struct RuntimePropertyIvarLayoutIndex {
  std::unordered_map<std::string, const EmittedIvarDescriptor *>
      ivars_by_binding_symbol;
  std::unordered_map<std::string, const EmittedIvarDescriptor *>
      ivars_by_layout_symbol;
  std::unordered_map<std::string, const EmittedIvarDescriptor *>
      ivars_by_layout_replay_key;
  std::unordered_set<std::string> ambiguous_layout_symbols;
  std::unordered_set<std::string> ambiguous_layout_replay_keys;
  std::size_t max_storage_end = 0u;
  std::size_t max_alignment = 1u;
  std::size_t published_owner_size = 0u;
};

bool BuildRuntimePropertyIvarLayoutIndex(
    const RegisteredImageMetadata &image,
    const std::string &ivar_owner_identity,
    RuntimePropertyIvarLayoutIndex &index);
std::size_t RuntimePropertyIvarLayoutInstanceSize(
    const RuntimePropertyIvarLayoutIndex &index);
const EmittedIvarDescriptor *FindRuntimePropertyIvarDescriptorForProperty(
    const RuntimePropertyIvarLayoutIndex &index,
    const EmittedPropertyDescriptor &descriptor);

}  // namespace objc3c::runtime
