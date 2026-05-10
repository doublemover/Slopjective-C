#include "artifacts/objc3_frontend_artifact_runtime_metadata_typed_bundles.h"

#include <algorithm>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

namespace objc3::artifacts::frontend {

bool BuildObjc3FrontendRuntimeMetadataIvarBundles(
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    std::vector<Objc3IRRuntimeMetadataIvarBundle> &ivar_bundles) {
  ivar_bundles.clear();
  ivar_bundles.reserve(source_graph.ivar_nodes_lexicographic.size());

  std::unordered_set<std::string> ivar_owner_identities;
  ivar_owner_identities.reserve(source_graph.ivar_nodes_lexicographic.size());
  for (const auto &ivar_node : source_graph.ivar_nodes_lexicographic) {
    if (ivar_node.owner_kind.empty() || ivar_node.owner_name.empty() ||
        ivar_node.owner_identity.empty() ||
        ivar_node.declaration_owner_identity.empty() ||
        ivar_node.export_owner_identity.empty() ||
        ivar_node.property_owner_identity.empty() ||
        ivar_node.property_name.empty() ||
        ivar_node.ivar_binding_symbol.empty() ||
        ivar_node.executable_synthesized_binding_kind.empty() ||
        ivar_node.executable_ivar_layout_symbol.empty() ||
        !ivar_owner_identities.insert(ivar_node.owner_identity).second) {
      return false;
    }

    Objc3IRRuntimeMetadataIvarBundle bundle;
    bundle.owner_kind = ivar_node.owner_kind;
    bundle.owner_name = ivar_node.owner_name;
    bundle.owner_identity = ivar_node.owner_identity;
    bundle.declaration_owner_identity = ivar_node.declaration_owner_identity;
    bundle.export_owner_identity = ivar_node.export_owner_identity;
    bundle.property_owner_identity = ivar_node.property_owner_identity;
    bundle.property_name = ivar_node.property_name;
    bundle.ivar_binding_symbol = ivar_node.ivar_binding_symbol;
    bundle.executable_synthesized_binding_kind =
        ivar_node.executable_synthesized_binding_kind;
    bundle.executable_synthesized_binding_symbol =
        ivar_node.executable_synthesized_binding_symbol;
    bundle.executable_ivar_layout_symbol =
        ivar_node.executable_ivar_layout_symbol;
    bundle.executable_ivar_layout_slot_index =
        ivar_node.executable_ivar_layout_slot_index;
    bundle.executable_ivar_layout_size_bytes =
        ivar_node.executable_ivar_layout_size_bytes;
    bundle.executable_ivar_layout_alignment_bytes =
        ivar_node.executable_ivar_layout_alignment_bytes;
    bundle.executable_ivar_layout_offset_bytes =
        ivar_node.executable_ivar_layout_offset_bytes;
    bundle.executable_ivar_layout_padding_bytes =
        ivar_node.executable_ivar_layout_padding_bytes;
    bundle.executable_ivar_layout_inherited_slot_count =
        ivar_node.executable_ivar_layout_inherited_slot_count;
    bundle.executable_ivar_layout_inherited_size_bytes =
        ivar_node.executable_ivar_layout_inherited_size_bytes;
    bundle.executable_ivar_layout_owner_size_bytes =
        ivar_node.executable_ivar_layout_owner_size_bytes;
    bundle.executable_ivar_init_order_index =
        ivar_node.executable_ivar_init_order_index;
    bundle.executable_ivar_destroy_order_index =
        ivar_node.executable_ivar_destroy_order_index;
    bundle.executable_ivar_layout_valid =
        ivar_node.executable_ivar_layout_valid;
    bundle.executable_ivar_layout_replay_key =
        ivar_node.executable_ivar_layout_replay_key;
    ivar_bundles.push_back(std::move(bundle));
  }

  std::sort(
      ivar_bundles.begin(), ivar_bundles.end(),
      [](const Objc3IRRuntimeMetadataIvarBundle &lhs,
         const Objc3IRRuntimeMetadataIvarBundle &rhs) {
        return std::tie(lhs.declaration_owner_identity, lhs.property_name,
                        lhs.owner_identity) <
               std::tie(rhs.declaration_owner_identity, rhs.property_name,
                        rhs.owner_identity);
      });
  return true;
}

}  // namespace objc3::artifacts::frontend
