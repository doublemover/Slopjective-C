#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <set>
#include <string>

#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"

namespace objc3::artifacts::frontend {

Objc3AccessorStorageLoweringMetadataSummary
BuildAccessorStorageLoweringMetadataSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  Objc3AccessorStorageLoweringMetadataSummary summary;
  summary.deterministic = records.deterministic;
  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.synthesizes_executable_accessors) {
      continue;
    }
    ++summary.synthesized_accessor_owner_entries;
    if (!property_record.getter_storage_runtime_helper_symbol.empty()) {
      ++summary.synthesized_getter_entries;
      if (property_record.getter_storage_runtime_helper_symbol ==
          kObjc3RuntimeReadCurrentPropertyI32Symbol) {
        ++summary.current_property_read_entries;
      } else if (property_record.getter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol) {
        ++summary.weak_current_property_load_entries;
      }
    }
    if (!property_record.setter_storage_runtime_helper_symbol.empty()) {
      ++summary.synthesized_setter_entries;
      if (property_record.setter_storage_runtime_helper_symbol ==
          kObjc3RuntimeWriteCurrentPropertyI32Symbol) {
        ++summary.current_property_write_entries;
      } else if (property_record.setter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeExchangeCurrentPropertyI32Symbol) {
        ++summary.current_property_exchange_entries;
      } else if (property_record.setter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol) {
        ++summary.weak_current_property_store_entries;
      }
    }
  }
  return summary;
}

Objc3ExecutableAccessorLayoutLoweringSummary
BuildExecutableAccessorLayoutLoweringSummary(
    const Objc3ExecutableMetadataSourceGraph &source_graph) {
  Objc3ExecutableAccessorLayoutLoweringSummary summary;
  summary.deterministic = source_graph.deterministic;
  std::set<std::string> ivar_layout_owner_identities;
  for (const auto &property_node : source_graph.property_nodes_lexicographic) {
    ++summary.property_metadata_entries;
    if (!property_node.property_attribute_profile.empty()) {
      ++summary.property_attribute_profile_entries;
    }
    if (!property_node.accessor_ownership_profile.empty()) {
      ++summary.accessor_ownership_profile_entries;
    }
    if (!property_node.executable_synthesized_binding_kind.empty()) {
      ++summary.synthesized_binding_entries;
    }
    if (property_node.owner_kind == "class-implementation" &&
        property_node.synthesizes_executable_accessors) {
      ++summary.implementation_owned_property_entries;
      if (!property_node.effective_getter_selector.empty()) {
        ++summary.synthesized_getter_entries;
        ++summary.synthesized_accessor_entries;
      }
      if (property_node.effective_setter_available &&
          !property_node.effective_setter_selector.empty()) {
        ++summary.synthesized_setter_entries;
        ++summary.synthesized_accessor_entries;
      }
    }
  }
  for (const auto &ivar_node : source_graph.ivar_nodes_lexicographic) {
    ++summary.ivar_metadata_entries;
    ++summary.ivar_layout_entries;
    if (!ivar_node.declaration_owner_identity.empty()) {
      ivar_layout_owner_identities.insert(ivar_node.declaration_owner_identity);
    }
  }
  summary.ivar_layout_owner_entries = ivar_layout_owner_identities.size();
  return summary;
}

}  // namespace objc3::artifacts::frontend
