#include "lower/contracts/typed_sema_lowering_boundary.h"

#include <sstream>

bool Objc3TypedSemaToLoweringBoundaryIsReady(
    const Objc3TypedSemaToLoweringBoundary &boundary) {
  return boundary.deterministic && boundary.all_callable_returns_typed &&
         boundary.all_params_have_concrete_type_surface &&
         boundary.diagnostics_clear && boundary.owner_split_explicit &&
         boundary.sema_to_lowering_owner_contract_recorded &&
         Objc3LoweringStrictOwnerModelIsReady(
             boundary.typed_semantic_handoff_owner,
             boundary.strict_contract_owner_model,
             boundary.strict_no_retired_route,
             boundary.strict_no_compatibility) &&
         Objc3LoweringStrictOwnerModelIsReady(
             boundary.lowering_consumer_owner,
             boundary.strict_contract_owner_model,
             boundary.strict_no_retired_route,
             boundary.strict_no_compatibility);
}

std::string Objc3TypedSemaToLoweringBoundaryReplayKey(
    const Objc3TypedSemaToLoweringBoundary &boundary) {
  std::ostringstream out;
  out << "ready=" << (Objc3TypedSemaToLoweringBoundaryIsReady(boundary)
                          ? "true"
                          : "false")
      << ";module=" << boundary.module_name
      << ";globals=" << boundary.global_value_sites
      << ";function_signatures=" << boundary.function_signature_sites
      << ";function_bodies=" << boundary.function_body_sites
      << ";function_param_slots=" << boundary.function_param_slots
      << ";method_signatures=" << boundary.method_signature_sites
      << ";method_bodies=" << boundary.method_body_sites
      << ";method_param_slots=" << boundary.method_param_slots
      << ";properties=" << boundary.property_sites
      << ";interfaces=" << boundary.interface_sites
      << ";protocols=" << boundary.protocol_sites
      << ";implementations=" << boundary.implementation_sites
      << ";concrete_param_types=" << boundary.concrete_param_type_surfaces
      << ";unknown_param_types=" << boundary.unknown_param_type_surfaces
      << ";object_reference_types=" << boundary.object_reference_type_sites
      << ";runtime_metadata_reference_types="
      << boundary.runtime_metadata_reference_type_sites
      << ";frontend_diagnostics=" << boundary.frontend_diagnostic_sites
      << ";deterministic=" << (boundary.deterministic ? "true" : "false")
      << ";returns_typed="
      << (boundary.all_callable_returns_typed ? "true" : "false")
      << ";params_typed="
      << (boundary.all_params_have_concrete_type_surface ? "true" : "false")
      << ";diagnostics_clear="
      << (boundary.diagnostics_clear ? "true" : "false")
      << ";owner_split_explicit="
      << (boundary.owner_split_explicit ? "true" : "false")
      << ";sema_to_lowering_owner_contract_recorded="
      << (boundary.sema_to_lowering_owner_contract_recorded ? "true" : "false")
      << ";lowering_consumer_owner=" << boundary.lowering_consumer_owner << ";"
      << Objc3LoweringOwnerReplayKey(
             boundary.typed_semantic_handoff_owner,
             boundary.strict_contract_owner_model,
             boundary.strict_no_retired_route,
             boundary.strict_no_compatibility);
  return out.str();
}
