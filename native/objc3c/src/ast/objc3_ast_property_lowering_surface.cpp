#include "ast/objc3_ast_property_lowering_surface.h"

#include <sstream>

#include "ast/objc3_ast_type_surface.h"

bool Objc3PropertyDeclHasRuntimeBackedStorage(
    const Objc3PropertyDecl &property) {
  return property.executable_ivar_layout_valid ||
         !property.executable_ivar_layout_symbol.empty() ||
         !property.ivar_binding_symbol.empty() ||
         !property.executable_synthesized_binding_symbol.empty();
}

bool Objc3PropertyDeclRequiresOwnershipRuntime(
    const Objc3PropertyDecl &property) {
  return property.is_copy || property.is_retain || property.is_strong ||
         property.is_weak || property.is_unowned ||
         property.is_unsafe_unretained || property.has_ownership_qualifier ||
         property.ownership_insert_retain || property.ownership_insert_release ||
         property.ownership_insert_autorelease;
}

std::string Objc3PropertyDeclLoweringReplayKey(
    const Objc3PropertyDecl &property) {
  std::ostringstream out;
  out << "property=" << property.name
      << ";type=" << Objc3ValueTypeSpelling(property.type)
      << ";runtime_storage="
      << (Objc3PropertyDeclHasRuntimeBackedStorage(property) ? "true" : "false")
      << ";ownership_runtime="
      << (Objc3PropertyDeclRequiresOwnershipRuntime(property) ? "true" : "false")
      << ";readonly=" << (property.is_readonly ? "true" : "false")
      << ";atomic=" << (property.is_atomic ? "true" : "false")
      << ";getter=" << property.effective_getter_selector
      << ";setter_available="
      << (property.effective_setter_available ? "true" : "false")
      << ";setter=" << property.effective_setter_selector
      << ";binding=" << property.executable_synthesized_binding_symbol
      << ";ivar=" << property.ivar_binding_symbol
      << ";layout=" << property.executable_ivar_layout_symbol
      << ";slot=" << property.executable_ivar_layout_slot_index
      << ";offset=" << property.executable_ivar_layout_offset_bytes
      << ";size=" << property.executable_ivar_layout_size_bytes
      << ";align=" << property.executable_ivar_layout_alignment_bytes;
  return out.str();
}
