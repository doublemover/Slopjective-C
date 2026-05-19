#include "artifacts/objc3_frontend_artifact_runtime_metadata_typed_bundles.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendRuntimeMetadataClassMetaclassBundles(
    Objc3IRFrontendRuntimeSourceClosureMetadata &runtime_source_metadata,
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
    runtime_source_metadata
        .runtime_metadata_class_metaclass_bundles_lexicographic =
        std::move(bundles);
  }
  runtime_source_metadata.runtime_metadata_class_metaclass_emission_ready =
      bundle_payload_complete;
  runtime_source_metadata
      .runtime_metadata_class_metaclass_emission_fail_closed =
      bundle_payload_complete;
}

}  // namespace objc3::artifacts::frontend
