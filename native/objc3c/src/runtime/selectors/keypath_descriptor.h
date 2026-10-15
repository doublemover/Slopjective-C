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

inline bool RuntimeKeyPathDebuggerMetadataIsMaterializable(
    const EmittedKeyPathDescriptor &descriptor) {
  return descriptor.component_owner_identity_path != nullptr &&
         descriptor.component_owner_identity_path[0] != '\0' &&
         descriptor.component_member_identity_path != nullptr &&
         descriptor.component_member_identity_path[0] != '\0' &&
         descriptor.component_type_identity_path != nullptr &&
         descriptor.component_type_identity_path[0] != '\0' &&
         descriptor.source_span_id != nullptr &&
         descriptor.source_span_id[0] != '\0' &&
         descriptor.root_type_identity != nullptr &&
         descriptor.root_type_identity[0] != '\0' &&
         descriptor.value_type_identity != nullptr &&
         descriptor.value_type_identity[0] != '\0' &&
         descriptor.object_model_owner_identity != nullptr &&
         descriptor.object_model_owner_identity[0] != '\0' &&
         descriptor.object_model_member_identity != nullptr &&
         descriptor.object_model_member_identity[0] != '\0' &&
         descriptor.debug_source_map_key != nullptr &&
         descriptor.debug_source_map_key[0] != '\0' &&
         descriptor.diagnostic_anchor_key != nullptr &&
         descriptor.diagnostic_anchor_key[0] != '\0' &&
         descriptor.source_line > 0 && descriptor.source_column > 0 &&
         !descriptor.fallback_interpretation_allowed;
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
         RuntimeKeyPathProfileIsMaterializable(descriptor.profile) &&
         RuntimeKeyPathDebuggerMetadataIsMaterializable(descriptor);
}

inline bool RuntimeKeyPathDescriptorMatchesSlot(
    const KeyPathSlot &slot,
    const EmittedKeyPathDescriptor &descriptor) {
  return slot.root_name_storage == descriptor.root_name &&
         slot.component_path_storage == descriptor.component_path &&
         slot.profile_storage == descriptor.profile &&
         slot.generic_metadata_replay_key_storage ==
             RuntimeKeyPathGenericMetadataReplayKey(descriptor) &&
         slot.component_owner_identity_path_storage ==
             descriptor.component_owner_identity_path &&
         slot.component_member_identity_path_storage ==
             descriptor.component_member_identity_path &&
         slot.component_type_identity_path_storage ==
             descriptor.component_type_identity_path &&
         slot.source_span_id_storage == descriptor.source_span_id &&
         slot.root_type_identity_storage == descriptor.root_type_identity &&
         slot.value_type_identity_storage == descriptor.value_type_identity &&
         slot.object_model_owner_identity_storage ==
             descriptor.object_model_owner_identity &&
         slot.object_model_member_identity_storage ==
             descriptor.object_model_member_identity &&
         slot.debug_source_map_key_storage == descriptor.debug_source_map_key &&
         slot.diagnostic_anchor_key_storage ==
             descriptor.diagnostic_anchor_key &&
         slot.source_line == descriptor.source_line &&
         slot.source_column == descriptor.source_column &&
         slot.root_is_self == descriptor.root_is_self &&
         slot.fallback_interpretation_allowed ==
             descriptor.fallback_interpretation_allowed;
}

}  // namespace objc3c::runtime
