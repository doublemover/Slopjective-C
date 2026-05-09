#include "ir/objc3_ir_property_metadata_comment_emission.h"

#include <cstddef>
#include <sstream>

#include "ir/objc3_ir_method_definition_plan.h"
#include "lower/contracts/executable_property_layout_contracts.h"
#include "lower/contracts/ownership_runtime_semantics_contracts.h"
#include "lower/contracts/runtime_property_layout_contracts.h"

void EmitObjc3IRPropertyMetadataCommentEmission(
    const Objc3IRPropertyMetadataCommentEmissionOptions &options,
    std::ostringstream &out) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary =
      options.lowering_ir_boundary;
  const std::size_t synthesized_property_accessor_count =
      options.synthesized_property_accessor_count;

  if (!frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key
           .empty()) {
    out << "; property_synthesis_ivar_binding_lowering = "
        << frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key
        << "\n";
    out << "; dispatch_and_synthesized_accessor_lowering_surface = "
        << "contract_id="
        << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
        << ";runtime_dispatch_symbol="
        << frontend_metadata.runtime_link_host_link_runtime_dispatch_symbol
        << ";runtime_dispatch_arg_slots="
        << frontend_metadata.runtime_link_host_link_runtime_dispatch_arg_slots
        << ";runtime_dispatch_symbol_matches_lowering="
        << ((frontend_metadata.runtime_link_host_link_runtime_dispatch_symbol ==
                     lowering_ir_boundary.runtime_dispatch_symbol &&
             frontend_metadata.runtime_link_host_link_runtime_dispatch_symbol ==
                 frontend_metadata
                     .runtime_support_library_link_wiring_runtime_dispatch_symbol)
                ? "true"
                : "false")
        << ";live_runtime_dispatch_sites="
        << (frontend_metadata.dispatch_surface_classification_instance_sites +
            frontend_metadata.dispatch_surface_classification_class_sites +
            frontend_metadata.dispatch_surface_classification_super_sites +
            frontend_metadata.dispatch_surface_classification_dynamic_sites)
        << ";direct_dispatch_sites="
        << frontend_metadata.dispatch_surface_classification_direct_sites
        << ";property_synthesis_sites="
        << frontend_metadata.lowering_property_synthesis_sites
        << ";property_synthesis_default_ivar_bindings="
        << frontend_metadata.lowering_property_synthesis_default_ivar_bindings
        << ";property_synthesis_explicit_ivar_bindings="
        << frontend_metadata.lowering_property_synthesis_explicit_ivar_bindings
        << ";interface_owned_property_synthesis_sites="
        << frontend_metadata.lowering_interface_owned_property_synthesis_sites
        << ";implementation_property_redeclaration_sites="
        << frontend_metadata
               .lowering_implementation_property_redeclaration_sites
        << ";ivar_binding_resolved="
        << frontend_metadata.lowering_property_synthesis_ivar_binding_resolved
        << ";property_descriptor_count="
        << frontend_metadata
               .runtime_metadata_section_publication_property_descriptor_count
        << ";ivar_descriptor_count="
        << frontend_metadata
               .runtime_metadata_section_publication_ivar_descriptor_count
        << ";member_table_emission_ready="
        << (frontend_metadata.runtime_metadata_member_table_emission_ready
                ? "true"
                : "false")
        << ";deterministic_handoff="
        << (frontend_metadata.lowering_property_synthesis_deterministic_handoff
                ? "true"
                : "false")
        << "\n";
  }

  if (frontend_metadata.executable_property_ivar_source_model_replay_key
          .empty()) {
    return;
  }

  out << "; property_ivar_source_model_completion = "
      << frontend_metadata.executable_property_ivar_source_model_replay_key
      << "\n";
  out << "; executable_property_accessor_layout_lowering = "
      << Objc3ExecutablePropertyAccessorLayoutLoweringSummary()
      << ";property_metadata_entries="
      << frontend_metadata.runtime_metadata_property_bundles_lexicographic
             .size()
      << ";ivar_metadata_entries="
      << frontend_metadata.runtime_metadata_ivar_bundles_lexicographic.size()
      << ";property_attribute_profiles="
      << frontend_metadata.executable_property_attribute_profile_entries
      << ";accessor_ownership_profiles="
      << frontend_metadata.executable_accessor_ownership_profile_entries
      << ";synthesized_binding_entries="
      << frontend_metadata.executable_synthesized_binding_entries
      << ";ivar_layout_entries="
      << frontend_metadata.executable_ivar_layout_entries << "\n";

  std::size_t ownership_lifetime_profile_entries = 0u;
  std::size_t ownership_runtime_hook_profile_entries = 0u;
  std::size_t runtime_backed_ownership_surface_entries = 0u;
  for (const auto &bundle :
       frontend_metadata.runtime_metadata_property_bundles_lexicographic) {
    const bool carries_runtime_backed_ownership_surface =
        !bundle.property_attribute_profile.empty() ||
        !bundle.ownership_lifetime_profile.empty() ||
        !bundle.ownership_runtime_hook_profile.empty() ||
        !bundle.accessor_ownership_profile.empty();
    if (carries_runtime_backed_ownership_surface) {
      ++runtime_backed_ownership_surface_entries;
    }
    if (!bundle.ownership_lifetime_profile.empty()) {
      ++ownership_lifetime_profile_entries;
    }
    if (!bundle.ownership_runtime_hook_profile.empty()) {
      ++ownership_runtime_hook_profile_entries;
    }
  }
  out << "; runtime_backed_object_ownership_attribute_surface = "
      << Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary()
      << ";property_descriptor_entries="
      << frontend_metadata.runtime_metadata_property_bundles_lexicographic
             .size()
      << ";runtime_backed_surface_entries="
      << runtime_backed_ownership_surface_entries
      << ";property_attribute_profiles="
      << frontend_metadata.executable_property_attribute_profile_entries
      << ";ownership_lifetime_profiles="
      << ownership_lifetime_profile_entries
      << ";ownership_runtime_hook_profiles="
      << ownership_runtime_hook_profile_entries
      << ";accessor_ownership_profiles="
      << frontend_metadata.executable_accessor_ownership_profile_entries
      << "\n";
  if (frontend_metadata.executable_ivar_layout_emission_ready) {
    out << "; executable_ivar_layout_emission = "
        << Objc3ExecutableIvarLayoutEmissionSummary()
        << ";offset_global_entries="
        << frontend_metadata.executable_ivar_offset_global_entries
        << ";layout_table_entries="
        << frontend_metadata.executable_ivar_layout_table_entries
        << ";layout_owner_entries="
        << frontend_metadata.executable_ivar_layout_owner_entries << "\n";
  }
  if (synthesized_property_accessor_count == 0u) {
    return;
  }

  out << "; executable_synthesized_accessor_property_lowering = "
      << Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary()
      << ";synthesized_accessor_entries="
      << synthesized_property_accessor_count << "\n";
  out << "; runtime_property_layout_consumption = "
      << Objc3RuntimePropertyLayoutConsumptionSummary()
      << ";property_descriptor_entries="
      << frontend_metadata.runtime_metadata_property_bundles_lexicographic
             .size()
      << ";ivar_layout_owner_entries="
      << frontend_metadata.executable_ivar_layout_owner_entries
      << ";synthesized_accessor_entries="
      << synthesized_property_accessor_count << "\n";
  out << "; runtime_instance_allocation_layout_support = "
      << Objc3RuntimeInstanceAllocationLayoutSupportSummary()
      << ";property_descriptor_entries="
      << frontend_metadata.runtime_metadata_property_bundles_lexicographic
             .size()
      << ";ivar_layout_owner_entries="
      << frontend_metadata.executable_ivar_layout_owner_entries
      << ";synthesized_accessor_entries="
      << synthesized_property_accessor_count << "\n";

  std::size_t writable_property_entries = 0u;
  for (const auto &bundle :
       frontend_metadata.runtime_metadata_property_bundles_lexicographic) {
    if (Objc3IRRuntimeMetadataPropertyBundleIsImplementationOwned(bundle) &&
        !bundle.executable_synthesized_binding_symbol.empty()) {
      writable_property_entries += bundle.effective_setter_available ? 1u : 0u;
    }
  }
  out << "; runtime_property_metadata_reflection = "
      << Objc3RuntimePropertyMetadataReflectionSummary()
      << ";reflectable_property_entries="
      << frontend_metadata.runtime_metadata_property_bundles_lexicographic
             .size()
      << ";writable_property_entries=" << writable_property_entries
      << ";synthesized_accessor_entries="
      << synthesized_property_accessor_count << "\n";
}
