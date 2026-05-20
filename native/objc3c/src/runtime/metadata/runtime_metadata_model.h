#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_contracts_source_property_metadata.h"
#include "runtime/metadata/runtime_ownership_contracts.h"

struct Objc3RuntimeMetadataClassSourceRecord {
  std::string record_kind;
  std::string name;
  std::string super_name;
  std::vector<std::string> adopted_protocols_lexicographic;
  bool has_super = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataProtocolSourceRecord {
  std::string name;
  std::vector<std::string> inherited_protocols_lexicographic;
  bool is_forward_declaration = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataCategorySourceRecord {
  std::string record_kind;
  std::string class_name;
  std::string category_name;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataPropertySourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string property_name;
  std::string type_name;
  bool has_getter = false;
  std::string getter_selector;
  bool has_setter = false;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  bool synthesizes_executable_accessors = false;
  std::string getter_storage_runtime_helper_symbol;
  std::string setter_storage_runtime_helper_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataMethodSourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string selector;
  bool is_class_method = false;
  bool has_body = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  std::size_t parameter_count = 0;
  std::string return_type_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataIvarSourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string property_name;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  std::string source_model = kObjc3RuntimeMetadataIvarSourceModel;
  unsigned line = 1;
  unsigned column = 1;
};

inline constexpr const char *kObjc3RuntimeExportLegalityContractId =
    "objc3c.runtime.export.legality.freeze.v1";
inline constexpr const char *kObjc3RuntimeExportEnforcementContractId =
    "objc3c.runtime.export.enforcement.v1";

struct Objc3RuntimeMetadataSourceRecordSet {
  std::string owner_split_contract_id =
      objc3c::runtime::kObjc3RuntimeOwnerSplitContractId;
  std::string metadata_model_owner =
      objc3c::runtime::kObjc3RuntimeMetadataModelOwner;
  std::string fail_closed_ownership_model =
      objc3c::runtime::kObjc3RuntimeFailClosedOwnershipModel;
  std::vector<Objc3RuntimeMetadataClassSourceRecord> classes_lexicographic;
  std::vector<Objc3RuntimeMetadataProtocolSourceRecord> protocols_lexicographic;
  std::vector<Objc3RuntimeMetadataCategorySourceRecord> categories_lexicographic;
  std::vector<Objc3RuntimeMetadataPropertySourceRecord> properties_lexicographic;
  std::vector<Objc3RuntimeMetadataMethodSourceRecord> methods_lexicographic;
  std::vector<Objc3RuntimeMetadataIvarSourceRecord> ivars_lexicographic;
  bool deterministic = false;
  bool metadata_model_owner_explicit = true;
};

inline bool IsReadyObjc3RuntimeMetadataSourceRecordSet(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return records.deterministic && !records.owner_split_contract_id.empty() &&
         !records.metadata_model_owner.empty() &&
         !records.fail_closed_ownership_model.empty() &&
         records.metadata_model_owner_explicit &&
         objc3c::runtime::RuntimeOwnerSplitContractIsReady();
}

struct Objc3RuntimeSupportLibraryLinkWiringSummary;
bool IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryLinkWiringSummary &summary);
