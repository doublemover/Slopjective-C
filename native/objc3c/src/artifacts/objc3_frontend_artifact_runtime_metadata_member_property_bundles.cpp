#include "artifacts/objc3_frontend_artifact_runtime_metadata_typed_bundles.h"

#include <algorithm>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

namespace objc3::artifacts::frontend {

bool BuildObjc3FrontendRuntimeMetadataPropertyBundles(
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    std::vector<Objc3IRRuntimeMetadataPropertyBundle> &property_bundles) {
  property_bundles.clear();
  property_bundles.reserve(source_graph.property_nodes_lexicographic.size());

  std::unordered_set<std::string> property_owner_identities;
  property_owner_identities.reserve(
      source_graph.property_nodes_lexicographic.size());
  for (const auto &property_node : source_graph.property_nodes_lexicographic) {
    if (property_node.owner_kind.empty() || property_node.owner_name.empty() ||
        property_node.owner_identity.empty() ||
        property_node.declaration_owner_identity.empty() ||
        property_node.export_owner_identity.empty() ||
        property_node.property_name.empty() || property_node.type_name.empty() ||
        !property_owner_identities.insert(property_node.owner_identity).second ||
        (property_node.has_getter && property_node.getter_selector.empty()) ||
        (property_node.has_setter && property_node.setter_selector.empty()) ||
        property_node.property_attribute_profile.empty() ||
        property_node.effective_getter_selector.empty() ||
        property_node.accessor_ownership_profile.empty() ||
        (property_node.effective_setter_available &&
         property_node.effective_setter_selector.empty())) {
      return false;
    }

    Objc3IRRuntimeMetadataPropertyBundle bundle;
    bundle.owner_kind = property_node.owner_kind;
    bundle.owner_name = property_node.owner_name;
    bundle.owner_identity = property_node.owner_identity;
    bundle.declaration_owner_identity =
        property_node.declaration_owner_identity;
    bundle.export_owner_identity = property_node.export_owner_identity;
    bundle.property_name = property_node.property_name;
    bundle.type_name = property_node.type_name;
    bundle.has_getter = property_node.has_getter;
    bundle.getter_selector = property_node.getter_selector;
    bundle.has_setter = property_node.has_setter;
    bundle.setter_selector = property_node.setter_selector;
    bundle.ivar_binding_symbol = property_node.ivar_binding_symbol;
    bundle.executable_synthesized_binding_kind =
        property_node.executable_synthesized_binding_kind;
    bundle.executable_synthesized_binding_symbol =
        property_node.executable_synthesized_binding_symbol;
    bundle.property_attribute_profile =
        property_node.property_attribute_profile;
    bundle.ownership_lifetime_profile =
        property_node.ownership_lifetime_profile;
    bundle.ownership_runtime_hook_profile =
        property_node.ownership_runtime_hook_profile;
    bundle.effective_getter_selector =
        property_node.effective_getter_selector;
    bundle.effective_setter_available =
        property_node.effective_setter_available;
    bundle.effective_setter_selector =
        property_node.effective_setter_selector;
    bundle.accessor_ownership_profile =
        property_node.accessor_ownership_profile;
    bundle.synthesizes_executable_accessors =
        property_node.synthesizes_executable_accessors;
    bundle.executable_ivar_layout_symbol =
        property_node.executable_ivar_layout_symbol;
    bundle.executable_ivar_layout_slot_index =
        property_node.executable_ivar_layout_slot_index;
    bundle.executable_ivar_layout_size_bytes =
        property_node.executable_ivar_layout_size_bytes;
    bundle.executable_ivar_layout_alignment_bytes =
        property_node.executable_ivar_layout_alignment_bytes;
    bundle.executable_ivar_layout_offset_bytes =
        property_node.executable_ivar_layout_offset_bytes;
    bundle.executable_ivar_layout_padding_bytes =
        property_node.executable_ivar_layout_padding_bytes;
    bundle.executable_ivar_layout_inherited_slot_count =
        property_node.executable_ivar_layout_inherited_slot_count;
    bundle.executable_ivar_layout_inherited_size_bytes =
        property_node.executable_ivar_layout_inherited_size_bytes;
    bundle.executable_ivar_layout_owner_size_bytes =
        property_node.executable_ivar_layout_owner_size_bytes;
    bundle.executable_ivar_init_order_index =
        property_node.executable_ivar_init_order_index;
    bundle.executable_ivar_destroy_order_index =
        property_node.executable_ivar_destroy_order_index;
    bundle.executable_ivar_layout_valid =
        property_node.executable_ivar_layout_valid;
    bundle.executable_ivar_layout_replay_key =
        property_node.executable_ivar_layout_replay_key;
    property_bundles.push_back(std::move(bundle));
  }

  std::sort(
      property_bundles.begin(), property_bundles.end(),
      [](const Objc3IRRuntimeMetadataPropertyBundle &lhs,
         const Objc3IRRuntimeMetadataPropertyBundle &rhs) {
        return std::tie(lhs.declaration_owner_identity, lhs.property_name,
                        lhs.owner_identity) <
               std::tie(rhs.declaration_owner_identity, rhs.property_name,
                        rhs.owner_identity);
      });
  return true;
}

}  // namespace objc3::artifacts::frontend
