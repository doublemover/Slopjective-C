#include "lower/contracts/object_model_lowering_contracts.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <string>

bool IsValidObjc3MethodLookupOverrideConflictContract(
    const Objc3MethodLookupOverrideConflictContract &contract) {
  if (contract.method_lookup_hits > contract.method_lookup_sites ||
      contract.method_lookup_misses > contract.method_lookup_sites ||
      contract.method_lookup_hits + contract.method_lookup_misses !=
          contract.method_lookup_sites) {
    return false;
  }
  if (contract.override_lookup_hits > contract.override_lookup_sites ||
      contract.override_lookup_misses > contract.override_lookup_sites ||
      contract.override_lookup_hits + contract.override_lookup_misses !=
          contract.override_lookup_sites) {
    return false;
  }
  if (contract.override_conflicts > contract.override_lookup_hits) {
    return false;
  }
  if (contract.unresolved_base_interfaces > contract.override_lookup_misses) {
    return false;
  }
  return true;
}

std::string Objc3MethodLookupOverrideConflictReplayKey(
    const Objc3MethodLookupOverrideConflictContract &contract) {
  return std::string("method_lookup_sites=") +
         std::to_string(contract.method_lookup_sites) +
         ";method_lookup_hits=" +
         std::to_string(contract.method_lookup_hits) +
         ";method_lookup_misses=" +
         std::to_string(contract.method_lookup_misses) +
         ";override_lookup_sites=" +
         std::to_string(contract.override_lookup_sites) +
         ";override_lookup_hits=" +
         std::to_string(contract.override_lookup_hits) +
         ";override_lookup_misses=" +
         std::to_string(contract.override_lookup_misses) +
         ";override_conflicts=" +
         std::to_string(contract.override_conflicts) +
         ";unresolved_base_interfaces=" +
         std::to_string(contract.unresolved_base_interfaces) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MethodLookupOverrideConflictLaneContract;
}

Objc3PropertySynthesisIvarBindingContract
Objc3DefaultPropertySynthesisIvarBindingContract(
    std::size_t property_synthesis_sites,
    bool deterministic) {
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings = 0;
  contract.property_synthesis_default_ivar_bindings = property_synthesis_sites;
  contract.interface_owned_property_synthesis_sites = property_synthesis_sites;
  contract.implementation_property_redeclaration_sites = 0;
  contract.ivar_binding_sites = property_synthesis_sites;
  contract.ivar_binding_resolved = property_synthesis_sites;
  contract.ivar_binding_missing = 0;
  contract.ivar_binding_conflicts = 0;
  contract.deterministic = deterministic;
  return contract;
}

bool IsValidObjc3PropertySynthesisIvarBindingContract(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  if (contract.property_synthesis_explicit_ivar_bindings +
              contract.property_synthesis_default_ivar_bindings !=
          contract.property_synthesis_sites ||
      contract.property_synthesis_explicit_ivar_bindings >
          contract.property_synthesis_sites ||
      contract.property_synthesis_default_ivar_bindings >
          contract.property_synthesis_sites) {
    return false;
  }
  if (contract.interface_owned_property_synthesis_sites !=
          contract.property_synthesis_sites ||
      contract.implementation_property_redeclaration_sites >
          contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_sites != contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_resolved > contract.ivar_binding_sites ||
      contract.ivar_binding_missing > contract.ivar_binding_sites ||
      contract.ivar_binding_conflicts > contract.ivar_binding_sites ||
      contract.ivar_binding_resolved + contract.ivar_binding_missing +
              contract.ivar_binding_conflicts !=
          contract.ivar_binding_sites) {
    return false;
  }
  return true;
}

std::string Objc3PropertySynthesisIvarBindingReplayKey(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  return std::string("property_synthesis_sites=") +
         std::to_string(contract.property_synthesis_sites) +
         ";property_synthesis_explicit_ivar_bindings=" +
         std::to_string(contract.property_synthesis_explicit_ivar_bindings) +
         ";property_synthesis_default_ivar_bindings=" +
         std::to_string(contract.property_synthesis_default_ivar_bindings) +
         ";interface_owned_property_synthesis_sites=" +
         std::to_string(contract.interface_owned_property_synthesis_sites) +
         ";implementation_property_redeclaration_sites=" +
         std::to_string(contract.implementation_property_redeclaration_sites) +
         ";ivar_binding_sites=" + std::to_string(contract.ivar_binding_sites) +
         ";ivar_binding_resolved=" +
         std::to_string(contract.ivar_binding_resolved) +
         ";ivar_binding_missing=" +
         std::to_string(contract.ivar_binding_missing) +
         ";ivar_binding_conflicts=" +
         std::to_string(contract.ivar_binding_conflicts) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3PropertySynthesisIvarBindingLaneContract;
}

bool IsValidObjc3IdClassSelObjectPointerTypecheckContract(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t computed_total =
      contract.id_typecheck_sites + contract.class_typecheck_sites +
      contract.sel_typecheck_sites + contract.object_pointer_typecheck_sites;
  return contract.total_typecheck_sites == computed_total;
}

std::string Objc3IdClassSelObjectPointerTypecheckReplayKey(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  return std::string("id_typecheck_sites=") +
         std::to_string(contract.id_typecheck_sites) +
         ";class_typecheck_sites=" +
         std::to_string(contract.class_typecheck_sites) +
         ";sel_typecheck_sites=" +
         std::to_string(contract.sel_typecheck_sites) +
         ";object_pointer_typecheck_sites=" +
         std::to_string(contract.object_pointer_typecheck_sites) +
         ";total_typecheck_sites=" +
         std::to_string(contract.total_typecheck_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3IdClassSelObjectPointerTypecheckLaneContract;
}
