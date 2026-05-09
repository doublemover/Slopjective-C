#pragma once

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"

#include <cstdint>

namespace objc3c::runtime {

inline bool RuntimeKeyPathHandleIsValid(std::uint64_t stable_id) {
  return stable_id != 0;
}

inline bool RuntimeKeyPathDescriptorIsMaterializable(
    const char *root_name,
    const char *component_path) {
  return root_name != nullptr && root_name[0] != '\0' &&
         component_path != nullptr && component_path[0] != '\0';
}

inline bool RuntimeKeyPathProfileIsMaterializable(const char *profile) {
  return profile != nullptr && profile[0] != '\0';
}

inline const char *RuntimeKeyPathGenericMetadataReplayKey(
    const EmittedKeyPathDescriptor &descriptor) {
  return descriptor.generic_metadata_replay_key == nullptr
             ? ""
             : descriptor.generic_metadata_replay_key;
}

inline std::uint64_t CountRuntimeKeyPathComponents(
    const char *component_path) {
  if (component_path == nullptr || component_path[0] == '\0') {
    return 0;
  }

  std::uint64_t count = 1;
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(component_path);
       *cursor != 0U; ++cursor) {
    if (*cursor == static_cast<unsigned char>('.')) {
      ++count;
    }
  }
  return count;
}

inline bool RuntimeKeyPathDescriptorIsComplete(
    const EmittedKeyPathDescriptor &descriptor) {
  return RuntimeKeyPathHandleIsValid(descriptor.stable_id) &&
         RuntimeKeyPathDescriptorIsMaterializable(descriptor.root_name,
                                                 descriptor.component_path) &&
         RuntimeKeyPathProfileIsMaterializable(descriptor.profile);
}

inline bool RuntimeKeyPathDescriptorMatchesSlot(
    const KeyPathSlot &slot,
    const EmittedKeyPathDescriptor &descriptor) {
  return slot.root_name_storage == descriptor.root_name &&
         slot.component_path_storage == descriptor.component_path &&
         slot.profile_storage == descriptor.profile &&
         slot.generic_metadata_replay_key_storage ==
             RuntimeKeyPathGenericMetadataReplayKey(descriptor) &&
         slot.root_is_self == descriptor.root_is_self;
}

}  // namespace objc3c::runtime
