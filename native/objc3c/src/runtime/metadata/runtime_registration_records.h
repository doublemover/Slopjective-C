#pragma once

#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/state/runtime_bootstrap_contracts.h"

#include <cstdint>
#include <string>

namespace objc3c::runtime {

struct SelectorSlot {
  std::string spelling_storage;
  objc3_runtime_selector_handle handle{};
  bool metadata_backed = false;
  std::uint64_t metadata_provider_count = 0;
  std::uint64_t first_registration_order_ordinal = 0;
  std::uint64_t last_registration_order_ordinal = 0;
  std::uint64_t first_selector_pool_index = 0;
  std::uint64_t last_selector_pool_index = 0;
};

struct KeyPathSlot {
  std::uint64_t stable_id = 0;
  std::string root_name_storage;
  std::string component_path_storage;
  std::string profile_storage;
  std::string generic_metadata_replay_key_storage;
  bool root_is_self = false;
  bool ambiguous = false;
  std::uint64_t component_count = 0;
  std::uint64_t metadata_provider_count = 0;
  std::uint64_t first_registration_order_ordinal = 0;
  std::uint64_t last_registration_order_ordinal = 0;
};

struct RegisteredImageMetadata {
  const char *owner_split_contract_id =
      kObjc3RuntimeOwnerSplitContractId;
  const char *metadata_model_owner = kObjc3RuntimeMetadataModelOwner;
  const char *registration_table_owner = kObjc3RuntimeRegistrationTableOwner;
  const char *manifest_descriptor_artifact_owner =
      kObjc3RuntimeManifestDescriptorArtifactOwner;
  const char *bootstrap_replay_owner = kObjc3RuntimeBootstrapReplayOwner;
  const char *fail_closed_ownership_model =
      kObjc3RuntimeFailClosedOwnershipModel;
  std::string module_name;
  std::string translation_unit_identity_key;
  std::uint64_t registration_order_ordinal = 0;
  const objc3_runtime_registration_table *registration_table = nullptr;
  const objc3_runtime_pointer_aggregate *discovery_root = nullptr;
  const objc3_runtime_pointer_aggregate *class_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *protocol_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *category_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *property_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *ivar_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *selector_pool_root = nullptr;
  const objc3_runtime_pointer_aggregate *string_pool_root = nullptr;
  const objc3_runtime_pointer_aggregate *keypath_descriptor_root = nullptr;
  std::uint64_t discovery_root_entry_count = 0;
  std::uint64_t class_descriptor_count = 0;
  std::uint64_t protocol_descriptor_count = 0;
  std::uint64_t category_descriptor_count = 0;
  std::uint64_t property_descriptor_count = 0;
  std::uint64_t ivar_descriptor_count = 0;
  std::uint64_t selector_pool_count = 0;
  std::uint64_t string_pool_count = 0;
  std::uint64_t keypath_descriptor_count = 0;
  bool linker_anchor_matches_discovery_root = false;
  bool used_staged_registration_table = false;
  bool ownership_explicit = true;
  bool fallback_path_allowed = false;
};

}  // namespace objc3c::runtime
