#include "artifacts/objc3_frontend_artifact_runtime_metadata_typed_bundles.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendRuntimeMetadataClassMetaclassBundles(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_name;
  class_nodes_by_name.reserve(source_graph.class_nodes_lexicographic.size());
  for (const auto &class_node : source_graph.class_nodes_lexicographic) {
    class_nodes_by_name.emplace(class_node.class_name, &class_node);
  }
  std::unordered_map<std::string,
                     const Objc3ExecutableMetadataMetaclassGraphNode *>
      metaclass_nodes_by_name;
  metaclass_nodes_by_name.reserve(
      source_graph.metaclass_nodes_lexicographic.size());
  for (const auto &metaclass_node :
       source_graph.metaclass_nodes_lexicographic) {
    metaclass_nodes_by_name.emplace(metaclass_node.class_name,
                                    &metaclass_node);
  }
  std::unordered_map<std::string,
                     const Objc3ExecutableMetadataImplementationGraphNode *>
      implementation_nodes_by_name;
  implementation_nodes_by_name.reserve(
      source_graph.implementation_nodes_lexicographic.size());
  for (const auto &implementation_node :
       source_graph.implementation_nodes_lexicographic) {
    implementation_nodes_by_name.emplace(implementation_node.class_name,
                                         &implementation_node);
  }

  bool bundle_payload_complete = true;
  std::vector<Objc3IRRuntimeMetadataClassMetaclassBundle> bundles;
  bundles.reserve(source_graph.interface_nodes_lexicographic.size() +
                  source_graph.implementation_nodes_lexicographic.size());
  for (const auto &interface_node :
       source_graph.interface_nodes_lexicographic) {
    const auto metaclass_it =
        metaclass_nodes_by_name.find(interface_node.class_name);
    const auto class_it = class_nodes_by_name.find(interface_node.class_name);
    if (metaclass_it == metaclass_nodes_by_name.end() ||
        class_it == class_nodes_by_name.end()) {
      bundle_payload_complete = false;
      break;
    }

    Objc3IRRuntimeMetadataClassMetaclassBundle bundle;
    bundle.class_name = interface_node.class_name;
    bundle.owner_identity = interface_node.owner_identity;
    bundle.class_owner_identity = interface_node.class_owner_identity;
    bundle.metaclass_owner_identity = interface_node.metaclass_owner_identity;
    bundle.has_super = class_it->second->has_super;
    bundle.super_class_owner_identity =
        class_it->second->super_class_owner_identity;
    bundle.super_metaclass_owner_identity =
        class_it->second->super_metaclass_owner_identity;
    bundle.super_bundle_owner_identity =
        class_it->second->has_super
            ? ("interface:" +
               class_it->second->super_class_owner_identity.substr(6))
            : std::string{};
    bundle.adopted_protocol_owner_identities_lexicographic =
        class_it->second->adopted_protocol_owner_identities_lexicographic;
    bundle.instance_method_owner_identity =
        interface_node.instance_method_owner_identity;
    bundle.class_method_owner_identity =
        interface_node.class_method_owner_identity;
    bundle.objc_final_declared = class_it->second->objc_final_declared;
    bundle.objc_sealed_declared = class_it->second->objc_sealed_declared;
    bundle.instance_method_count = interface_node.instance_method_count;
    bundle.class_method_count =
        metaclass_it->second->interface_class_method_count;
    bundles.push_back(std::move(bundle));
  }
  for (const auto &implementation_node :
       source_graph.implementation_nodes_lexicographic) {
    const auto metaclass_it =
        metaclass_nodes_by_name.find(implementation_node.class_name);
    const auto class_it =
        class_nodes_by_name.find(implementation_node.class_name);
    if (metaclass_it == metaclass_nodes_by_name.end() ||
        class_it == class_nodes_by_name.end()) {
      bundle_payload_complete = false;
      break;
    }

    Objc3IRRuntimeMetadataClassMetaclassBundle bundle;
    bundle.class_name = implementation_node.class_name;
    bundle.owner_identity = implementation_node.owner_identity;
    bundle.class_owner_identity = implementation_node.class_owner_identity;
    bundle.metaclass_owner_identity =
        implementation_node.metaclass_owner_identity;
    bundle.has_super = class_it->second->has_super;
    bundle.super_class_owner_identity =
        implementation_node.super_class_owner_identity;
    bundle.super_metaclass_owner_identity =
        implementation_node.super_metaclass_owner_identity;
    if (class_it->second->has_super) {
      const std::string super_class_name =
          class_it->second->super_class_owner_identity.substr(6);
      const auto super_impl_it =
          implementation_nodes_by_name.find(super_class_name);
      if (super_impl_it == implementation_nodes_by_name.end()) {
        bundle_payload_complete = false;
        break;
      }
      bundle.super_bundle_owner_identity =
          super_impl_it->second->owner_identity;
    }
    bundle.adopted_protocol_owner_identities_lexicographic =
        class_it->second->adopted_protocol_owner_identities_lexicographic;
    bundle.instance_method_owner_identity =
        implementation_node.instance_method_owner_identity;
    bundle.class_method_owner_identity =
        implementation_node.class_method_owner_identity;
    bundle.objc_final_declared = class_it->second->objc_final_declared;
    bundle.objc_sealed_declared = class_it->second->objc_sealed_declared;
    bundle.instance_method_count = implementation_node.instance_method_count;
    bundle.class_method_count =
        metaclass_it->second->implementation_class_method_count;
    bundles.push_back(std::move(bundle));
  }

  bundle_payload_complete =
      bundle_payload_complete &&
      bundles.size() ==
          runtime_metadata_section_publication.class_descriptor_count;
  if (bundle_payload_complete) {
    ir_frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic =
        std::move(bundles);
  }
  ir_frontend_metadata.runtime_metadata_class_metaclass_emission_ready =
      bundle_payload_complete;
  ir_frontend_metadata.runtime_metadata_class_metaclass_emission_fail_closed =
      bundle_payload_complete;
}

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
