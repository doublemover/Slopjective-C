#include "ast/objc3_ast_decl_surface.h"

#include "ast/objc3_ast_type_surface.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace {

std::vector<std::string> BuildSortedUniqueStrings(std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

}  // namespace

bool Objc3MethodDeclHasRuntimeBody(const Objc3MethodDecl &method) {
  return method.has_body && !method.body.empty();
}

bool Objc3MethodDeclRequiresRuntimeDispatch(const Objc3MethodDecl &method) {
  if (method.objc_direct_declared) {
    return false;
  }
  return method.is_class_method || method.has_body || method.objc_dynamic_declared ||
         method.objc_final_declared;
}

std::string Objc3MethodSignatureReplayKey(const Objc3MethodDecl &method) {
  std::ostringstream out;
  out << Objc3CallableSignatureReplayKey(method.selector, method.params,
                                         method.return_type,
                                         method.async_declared,
                                         method.throws_declared)
      << ";class_method=" << (method.is_class_method ? "true" : "false")
      << ";selector_normalized="
      << (method.selector_is_normalized ? "true" : "false")
      << ";selector_piece_count=" << method.selector_pieces.size()
      << ";direct=" << (method.objc_direct_declared ? "true" : "false")
      << ";dynamic=" << (method.objc_dynamic_declared ? "true" : "false")
      << ";final=" << (method.objc_final_declared ? "true" : "false")
      << ";body=" << (Objc3MethodDeclHasRuntimeBody(method) ? "true" : "false")
      << ";lookup=" << method.method_lookup_symbol
      << ";owner=" << method.scope_owner_symbol;
  return out.str();
}

std::string Objc3FunctionSignatureReplayKey(const FunctionDecl &function) {
  std::ostringstream out;
  out << Objc3CallableSignatureReplayKey(function.name, function.params,
                                         function.return_type,
                                         function.async_declared,
                                         function.throws_declared)
      << ";prototype=" << (function.is_prototype ? "true" : "false")
      << ";pure=" << (function.is_pure ? "true" : "false")
      << ";body=" << (!function.body.empty() ? "true" : "false")
      << ";owner=" << function.scope_owner_symbol;
  return out.str();
}

bool Objc3PropertyDeclHasRuntimeBackedStorage(const Objc3PropertyDecl &property) {
  return property.executable_ivar_layout_valid ||
         !property.executable_ivar_layout_symbol.empty() ||
         !property.ivar_binding_symbol.empty() ||
         !property.executable_synthesized_binding_symbol.empty();
}

bool Objc3PropertyDeclRequiresOwnershipRuntime(const Objc3PropertyDecl &property) {
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

std::size_t Objc3ProgramDeclarationCount(const Objc3Program &program) {
  return program.globals.size() + program.protocols.size() +
         program.interfaces.size() + program.implementations.size() +
         program.functions.size();
}

std::string Objc3ProgramLoweringReplayKey(const Objc3Program &program) {
  std::ostringstream out;
  out << "module=" << program.module_name
      << ";globals=" << program.globals.size()
      << ";protocols=" << program.protocols.size()
      << ";interfaces=" << program.interfaces.size()
      << ";implementations=" << program.implementations.size()
      << ";functions=" << program.functions.size()
      << ";decls=" << Objc3ProgramDeclarationCount(program)
      << ";diagnostics=" << program.diagnostics.size();
  return out.str();
}

std::vector<std::string> BuildScopePathLexicographic(std::string owner_symbol,
                                                     std::string entry_symbol) {
  std::vector<std::string> path;
  if (!owner_symbol.empty()) {
    path.push_back(std::move(owner_symbol));
  }
  if (!entry_symbol.empty()) {
    path.push_back(std::move(entry_symbol));
  }
  return BuildSortedUniqueStrings(std::move(path));
}

std::string BuildObjcContainerScopeOwner(const std::string &container_kind,
                                         const std::string &name,
                                         bool has_category,
                                         const std::string &category_name) {
  std::string owner = container_kind + ":" + name;
  if (has_category) {
    owner += "(" + category_name + ")";
  }
  return owner;
}

std::string BuildObjcMethodScopePathSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_method:" : "instance_method:") + method.selector;
}

std::string BuildObjcPropertyScopePathSymbol(const Objc3PropertyDecl &property) {
  return "property:" + property.name;
}

std::string BuildObjcPropertySynthesisSymbol(const Objc3PropertyDecl &property) {
  return "property_synthesis:" + property.name;
}

std::string BuildObjcIvarBindingSymbol(const Objc3PropertyDecl &property) {
  return "ivar_binding:_" + property.name;
}

std::string BuildObjcTypecheckParamFamilySymbol(const FuncParam &param) {
  if (param.id_spelling) {
    return "id";
  }
  if (param.class_spelling) {
    return "Class";
  }
  if (param.sel_spelling) {
    return "SEL";
  }
  if (param.object_pointer_type_spelling) {
    return "object-pointer:" + param.object_pointer_type_name;
  }
  return "";
}

std::string BuildObjcTypecheckReturnFamilySymbol(const FunctionDecl &fn) {
  if (fn.return_id_spelling) {
    return "id";
  }
  if (fn.return_class_spelling) {
    return "Class";
  }
  if (fn.return_sel_spelling) {
    return "SEL";
  }
  if (fn.return_object_pointer_type_spelling) {
    return "object-pointer:" + fn.return_object_pointer_type_name;
  }
  return "";
}

std::vector<std::string> BuildProtocolSemanticLinkTargetsLexicographic(
    const std::vector<std::string> &protocol_names) {
  std::vector<std::string> targets;
  targets.reserve(protocol_names.size());
  for (const auto &name : protocol_names) {
    if (!name.empty()) {
      targets.push_back("protocol:" + name);
    }
  }
  return BuildSortedUniqueStrings(std::move(targets));
}

std::string BuildObjcCategorySemanticLinkSymbol(const std::string &owner_name,
                                                const std::string &category_name) {
  return "category:" + owner_name + "(" + category_name + ")";
}

std::string BuildObjcMethodLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_lookup:" : "instance_lookup:") + method.selector;
}

std::string BuildObjcMethodOverrideLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_override:" : "instance_override:") + method.selector;
}

std::string BuildObjcMethodConflictLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_conflict:" : "instance_conflict:") + method.selector;
}

std::vector<std::string> BuildObjcMethodLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.method_lookup_symbol.empty()) {
      symbols.push_back(method.method_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

std::vector<std::string> BuildObjcMethodOverrideLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.override_lookup_symbol.empty()) {
      symbols.push_back(method.override_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

std::vector<std::string> BuildObjcMethodConflictLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.conflict_lookup_symbol.empty()) {
      symbols.push_back(method.conflict_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

std::vector<std::string> BuildObjcPropertySynthesisSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties) {
  std::vector<std::string> symbols;
  symbols.reserve(properties.size());
  for (const auto &property : properties) {
    if (!property.property_synthesis_symbol.empty()) {
      symbols.push_back(property.property_synthesis_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

std::vector<std::string> BuildObjcIvarBindingSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties) {
  std::vector<std::string> symbols;
  symbols.reserve(properties.size());
  for (const auto &property : properties) {
    if (!property.ivar_binding_symbol.empty()) {
      symbols.push_back(property.ivar_binding_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}
