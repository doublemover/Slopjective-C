  std::vector<std::string> interface_names;
  interface_names.reserve(surface.interfaces.size());
  for (const auto &entry : surface.interfaces) {
    interface_names.push_back(entry.first);
  }
  std::sort(interface_names.begin(), interface_names.end());

  handoff.interfaces_lexicographic.reserve(interface_names.size());
  // ARC lowering ABI/cleanup freeze anchor: sema continues to own
  // ARC legality, inferred lifetime packets, and synthesized property ownership
  // metadata only; it does not schedule cleanups or choose lowering helper
  // placement.
  // ARC automatic-insertion implementation anchor: sema now also
  // exports the supported ARC insertion flags and normalized property ownership
  // metadata that lane C consumes for helper placement, while helper emission
  // itself remains lowering-owned.
  const auto apply_arc_property_interaction_metadata =
      [](Objc3SemanticPropertyTypeMetadata &property_metadata) {
        const objc3c::support::Objc3PropertyOwnershipProfileKind
            ownership_profile_kind =
                objc3c::support::ApplyPropertyOwnershipProfileRetiredRoute(
                    property_metadata, false);
        if (ownership_profile_kind ==
            objc3c::support::Objc3PropertyOwnershipProfileKind::kWeak) {
          property_metadata.ownership_is_weak_reference = true;
        }
        objc3c::support::RebuildPropertyAccessorOwnershipProfileIfNeeded(
            property_metadata);
      };
  for (const std::string &name : interface_names) {
    const auto interface_it = surface.interfaces.find(name);
    if (interface_it == surface.interfaces.end()) {
      continue;
    }

    Objc3SemanticInterfaceTypeMetadata metadata;
    metadata.name = name;
    metadata.super_name = interface_it->second.super_name;
    metadata.generic_parameter_names_source_order =
        interface_it->second.generic_parameter_names_source_order;
    metadata.generic_parameter_variance_source_order =
        interface_it->second.generic_parameter_variance_source_order;
    metadata.adopted_protocols_lexicographic =
        interface_it->second.adopted_protocols_lexicographic;

#include "sema/objc3_semantic_passes_type_metadata_interface_properties.inc"

#include "sema/objc3_semantic_passes_type_metadata_interface_methods.inc"

    handoff.interfaces_lexicographic.push_back(std::move(metadata));
  }
