#include "sema/objc3_sema_symbol_binding_equivalence.h"

bool IsEquivalentSymbolGraphScopeResolutionSummary(
    const Objc3SymbolGraphScopeResolutionSummary &lhs,
    const Objc3SymbolGraphScopeResolutionSummary &rhs) {
  return lhs.global_symbol_nodes == rhs.global_symbol_nodes &&
         lhs.function_symbol_nodes == rhs.function_symbol_nodes &&
         lhs.interface_symbol_nodes == rhs.interface_symbol_nodes &&
         lhs.implementation_symbol_nodes == rhs.implementation_symbol_nodes &&
         lhs.interface_property_symbol_nodes == rhs.interface_property_symbol_nodes &&
         lhs.implementation_property_symbol_nodes == rhs.implementation_property_symbol_nodes &&
         lhs.interface_method_symbol_nodes == rhs.interface_method_symbol_nodes &&
         lhs.implementation_method_symbol_nodes == rhs.implementation_method_symbol_nodes &&
         lhs.top_level_scope_symbols == rhs.top_level_scope_symbols &&
         lhs.nested_scope_symbols == rhs.nested_scope_symbols &&
         lhs.scope_frames_total == rhs.scope_frames_total &&
         lhs.implementation_interface_resolution_sites == rhs.implementation_interface_resolution_sites &&
         lhs.implementation_interface_resolution_hits == rhs.implementation_interface_resolution_hits &&
         lhs.implementation_interface_resolution_misses == rhs.implementation_interface_resolution_misses &&
         lhs.method_resolution_sites == rhs.method_resolution_sites &&
         lhs.method_resolution_hits == rhs.method_resolution_hits &&
         lhs.method_resolution_misses == rhs.method_resolution_misses;
}

bool IsEquivalentClassProtocolCategoryLinkingSummary(
    const Objc3ClassProtocolCategoryLinkingSummary &lhs,
    const Objc3ClassProtocolCategoryLinkingSummary &rhs) {
  return lhs.declared_interfaces == rhs.declared_interfaces &&
         lhs.resolved_interfaces == rhs.resolved_interfaces &&
         lhs.declared_implementations == rhs.declared_implementations &&
         lhs.resolved_implementations == rhs.resolved_implementations &&
         lhs.interface_method_symbols == rhs.interface_method_symbols &&
         lhs.implementation_method_symbols == rhs.implementation_method_symbols &&
         lhs.linked_implementation_symbols == rhs.linked_implementation_symbols &&
         lhs.protocol_composition_sites == rhs.protocol_composition_sites &&
         lhs.protocol_composition_symbols == rhs.protocol_composition_symbols &&
         lhs.category_composition_sites == rhs.category_composition_sites &&
         lhs.category_composition_symbols == rhs.category_composition_symbols &&
         lhs.invalid_protocol_composition_sites == rhs.invalid_protocol_composition_sites;
}

bool IsEquivalentMethodLookupOverrideConflictSummary(
    const Objc3MethodLookupOverrideConflictSummary &lhs,
    const Objc3MethodLookupOverrideConflictSummary &rhs) {
  return lhs.method_lookup_sites == rhs.method_lookup_sites &&
         lhs.method_lookup_hits == rhs.method_lookup_hits &&
         lhs.method_lookup_misses == rhs.method_lookup_misses &&
         lhs.override_lookup_sites == rhs.override_lookup_sites &&
         lhs.override_lookup_hits == rhs.override_lookup_hits &&
         lhs.override_lookup_misses == rhs.override_lookup_misses &&
         lhs.override_conflicts == rhs.override_conflicts &&
         lhs.unresolved_base_interfaces == rhs.unresolved_base_interfaces;
}

bool IsEquivalentPropertySynthesisIvarBindingSummary(
    const Objc3PropertySynthesisIvarBindingSummary &lhs,
    const Objc3PropertySynthesisIvarBindingSummary &rhs) {
  return lhs.property_synthesis_sites == rhs.property_synthesis_sites &&
         lhs.property_synthesis_explicit_ivar_bindings == rhs.property_synthesis_explicit_ivar_bindings &&
         lhs.property_synthesis_default_ivar_bindings == rhs.property_synthesis_default_ivar_bindings &&
         lhs.ivar_binding_sites == rhs.ivar_binding_sites &&
         lhs.ivar_binding_resolved == rhs.ivar_binding_resolved &&
         lhs.ivar_binding_missing == rhs.ivar_binding_missing &&
         lhs.ivar_binding_conflicts == rhs.ivar_binding_conflicts;
}

bool IsEquivalentIdClassSelObjectPointerTypeCheckingSummary(
    const Objc3IdClassSelObjectPointerTypeCheckingSummary &lhs,
    const Objc3IdClassSelObjectPointerTypeCheckingSummary &rhs) {
  return lhs.param_type_sites == rhs.param_type_sites &&
         lhs.param_id_spelling_sites == rhs.param_id_spelling_sites &&
         lhs.param_class_spelling_sites == rhs.param_class_spelling_sites &&
         lhs.param_sel_spelling_sites == rhs.param_sel_spelling_sites &&
         lhs.param_instancetype_spelling_sites == rhs.param_instancetype_spelling_sites &&
         lhs.param_object_pointer_type_sites == rhs.param_object_pointer_type_sites &&
         lhs.return_type_sites == rhs.return_type_sites &&
         lhs.return_id_spelling_sites == rhs.return_id_spelling_sites &&
         lhs.return_class_spelling_sites == rhs.return_class_spelling_sites &&
         lhs.return_sel_spelling_sites == rhs.return_sel_spelling_sites &&
         lhs.return_instancetype_spelling_sites == rhs.return_instancetype_spelling_sites &&
         lhs.return_object_pointer_type_sites == rhs.return_object_pointer_type_sites &&
         lhs.property_type_sites == rhs.property_type_sites &&
         lhs.property_id_spelling_sites == rhs.property_id_spelling_sites &&
         lhs.property_class_spelling_sites == rhs.property_class_spelling_sites &&
         lhs.property_sel_spelling_sites == rhs.property_sel_spelling_sites &&
         lhs.property_instancetype_spelling_sites == rhs.property_instancetype_spelling_sites &&
         lhs.property_object_pointer_type_sites == rhs.property_object_pointer_type_sites;
}
