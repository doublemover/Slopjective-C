#include "artifacts/objc3_frontend_artifact_runtime_metadata_typed_bundles.h"

#include <cstddef>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace objc3::artifacts::frontend {

bool ApplyObjc3FrontendRuntimeMetadataProtocolCategoryBundles(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  bool protocol_category_payload_complete = true;
  std::unordered_set<std::string> protocol_owner_identities;
  protocol_owner_identities.reserve(
      source_graph.protocol_nodes_lexicographic.size());
  std::vector<Objc3IRRuntimeMetadataProtocolBundle> protocol_bundles;
  protocol_bundles.reserve(source_graph.protocol_nodes_lexicographic.size());
  for (const auto &protocol_node : source_graph.protocol_nodes_lexicographic) {
    if (protocol_node.protocol_name.empty() ||
        protocol_node.owner_identity.empty() ||
        !protocol_owner_identities.insert(protocol_node.owner_identity)
             .second) {
      protocol_category_payload_complete = false;
      break;
    }
    for (const auto &inherited_owner_identity :
         protocol_node.inherited_protocol_owner_identities_lexicographic) {
      if (inherited_owner_identity.empty()) {
        protocol_category_payload_complete = false;
        break;
      }
    }
    if (!protocol_category_payload_complete) {
      break;
    }

    Objc3IRRuntimeMetadataProtocolBundle bundle;
    bundle.protocol_name = protocol_node.protocol_name;
    bundle.owner_identity = protocol_node.owner_identity;
    bundle.inherited_protocol_owner_identities_lexicographic =
        protocol_node.inherited_protocol_owner_identities_lexicographic;
    bundle.property_count = protocol_node.property_count;
    bundle.method_count = protocol_node.method_count;
    bundle.is_forward_declaration = protocol_node.is_forward_declaration;
    protocol_bundles.push_back(std::move(bundle));
  }

  std::vector<Objc3IRRuntimeMetadataCategoryBundle> category_bundles;
  category_bundles.reserve(source_graph.category_nodes_lexicographic.size());
  std::unordered_set<std::string> category_owner_identities;
  category_owner_identities.reserve(
      runtime_metadata_section_publication.category_descriptor_count);
  if (protocol_category_payload_complete) {
    for (const auto &category_node :
         source_graph.category_nodes_lexicographic) {
      if (category_node.class_name.empty() ||
          category_node.category_name.empty() ||
          category_node.owner_identity.empty() ||
          category_node.class_owner_identity.empty() ||
          (!category_node.has_interface &&
           !category_node.has_implementation)) {
        protocol_category_payload_complete = false;
        break;
      }

      for (const auto &protocol_owner_identity :
           category_node.adopted_protocol_owner_identities_lexicographic) {
        if (protocol_owner_identity.empty() ||
            protocol_owner_identities.find(protocol_owner_identity) ==
                protocol_owner_identities.end()) {
          protocol_category_payload_complete = false;
          break;
        }
      }
      if (!protocol_category_payload_complete) {
        break;
      }

      const auto append_category_bundle =
          [&](const std::string &record_kind,
              const std::string &record_owner_identity,
              std::size_t property_count, std::size_t instance_method_count,
              std::size_t class_method_count) {
            if (record_owner_identity.empty() ||
                !category_owner_identities.insert(record_owner_identity)
                     .second) {
              protocol_category_payload_complete = false;
              return;
            }
            Objc3IRRuntimeMetadataCategoryBundle bundle;
            bundle.record_kind = record_kind;
            bundle.class_name = category_node.class_name;
            bundle.category_name = category_node.category_name;
            bundle.owner_identity = record_owner_identity;
            bundle.category_owner_identity = category_node.owner_identity;
            bundle.class_owner_identity = category_node.class_owner_identity;
            bundle.adopted_protocol_owner_identities_lexicographic =
                category_node.adopted_protocol_owner_identities_lexicographic;
            bundle.property_count = property_count;
            bundle.instance_method_count = instance_method_count;
            bundle.class_method_count = class_method_count;
            category_bundles.push_back(std::move(bundle));
          };
      if (category_node.has_interface) {
        append_category_bundle("interface",
                               category_node.interface_owner_identity,
                               category_node.interface_property_count,
                               category_node.interface_method_count,
                               category_node.interface_class_method_count);
      }
      if (protocol_category_payload_complete &&
          category_node.has_implementation) {
        append_category_bundle("implementation",
                               category_node.implementation_owner_identity,
                               category_node.implementation_property_count,
                               category_node.implementation_method_count,
                               category_node
                                   .implementation_class_method_count);
      }
      if (!protocol_category_payload_complete) {
        break;
      }
    }
  }

  if (protocol_category_payload_complete) {
    for (const auto &bundle : protocol_bundles) {
      for (const auto &inherited_owner_identity :
           bundle.inherited_protocol_owner_identities_lexicographic) {
        if (protocol_owner_identities.find(inherited_owner_identity) ==
            protocol_owner_identities.end()) {
          protocol_category_payload_complete = false;
          break;
        }
      }
      if (!protocol_category_payload_complete) {
        break;
      }
    }
  }

  protocol_category_payload_complete =
      protocol_category_payload_complete &&
      protocol_bundles.size() ==
          runtime_metadata_section_publication.protocol_descriptor_count &&
      category_bundles.size() ==
          runtime_metadata_section_publication.category_descriptor_count;
  if (protocol_category_payload_complete) {
    ir_frontend_metadata.runtime_metadata_protocol_bundles_lexicographic =
        std::move(protocol_bundles);
    ir_frontend_metadata.runtime_metadata_category_bundles_lexicographic =
        std::move(category_bundles);
  }
  ir_frontend_metadata.runtime_metadata_protocol_category_emission_ready =
      protocol_category_payload_complete;
  ir_frontend_metadata.runtime_metadata_protocol_category_emission_fail_closed =
      protocol_category_payload_complete;
  return protocol_category_payload_complete;
}

}  // namespace objc3::artifacts::frontend
