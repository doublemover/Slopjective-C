#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "ast/objc3_ast_declarations.h"

namespace objc3::artifacts::frontend {
namespace {

std::string SanitizeMetaprogrammingArtifactToken(const std::string &text) {
  std::string out;
  out.reserve(text.size());
  for (unsigned char ch : text) {
    if (std::isalnum(ch) != 0) {
      out.push_back(static_cast<char>(ch));
    } else {
      out.push_back('_');
    }
  }
  if (out.empty()) {
    out = "anonymous";
  }
  return out;
}

std::string BuildMetaprogrammingRuntimeClassOwnerIdentity(
    const std::string &class_name) {
  return "class:" + class_name;
}

std::string BuildMetaprogrammingRuntimeCategoryOwnerIdentity(
    const std::string &class_name, const std::string &category_name) {
  return "category:" + class_name + "(" + category_name + ")";
}

std::string BuildMetaprogrammingCategoryOwnerName(
    const std::string &class_name,
    const std::string &category_name) {
  return class_name + "(" + category_name + ")";
}

std::string BuildMetaprogrammingDerivedMethodFunctionSymbol(
    const std::string &implementation_name, const std::string &selector) {
  return "objc3_method_" +
         SanitizeMetaprogrammingArtifactToken(implementation_name) +
         "_instance_" + SanitizeMetaprogrammingArtifactToken(selector);
}

std::string BuildMetaprogrammingMacroArtifactSymbol(
    const std::string &function_name, const std::string &macro_name) {
  return "objc3_metaprogramming_macro_artifact_" +
         SanitizeMetaprogrammingArtifactToken(function_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(macro_name);
}

std::string BuildMetaprogrammingPropertyBehaviorArtifactSymbol(
    const std::string &owner_kind, const std::string &owner_name,
    const std::string &property_name, const std::string &behavior_name) {
  return "objc3_metaprogramming_property_behavior_" +
         SanitizeMetaprogrammingArtifactToken(owner_kind) + "_" +
         SanitizeMetaprogrammingArtifactToken(owner_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(property_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(behavior_name);
}

std::string NormalizeMetaprogrammingNamedStringToken(std::string token) {
  token.erase(std::remove_if(token.begin(), token.end(),
                             [](unsigned char ch) {
                               return std::isspace(ch) != 0;
                             }),
              token.end());
  if (token.size() >= 2u && token.front() == '"' && token.back() == '"') {
    token = token.substr(1u, token.size() - 2u);
  }
  return token;
}

std::string NormalizeMetaprogrammingPropertyBehaviorNameForEmission(
    std::string token) {
  token = NormalizeMetaprogrammingNamedStringToken(std::move(token));
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  if (token == "observed") {
    return "Observed";
  }
  if (token == "projected") {
    return "Projected";
  }
  return {};
}

std::string NormalizeMetaprogrammingDeriveNameForEmission(std::string token) {
  token = NormalizeMetaprogrammingNamedStringToken(std::move(token));
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  if (token == "equality" || token == "equatable") {
    return "Equality";
  }
  if (token == "hash") {
    return "Hash";
  }
  if (token == "debugdescription") {
    return "DebugDescription";
  }
  return {};
}

bool IsSupportedMetaprogrammingMacroPackageNameForEmission(
    std::string package_name) {
  package_name = NormalizeMetaprogrammingNamedStringToken(std::move(package_name));
  std::transform(package_name.begin(), package_name.end(), package_name.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  return package_name == "std.metaprogramming" ||
         package_name == "std.metaprogramming.trace";
}

bool IsSupportedMetaprogrammingMacroProvenanceNameForEmission(
    std::string provenance_name) {
  provenance_name =
      NormalizeMetaprogrammingNamedStringToken(std::move(provenance_name));
  std::transform(provenance_name.begin(), provenance_name.end(),
                 provenance_name.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  constexpr std::string_view prefix = "sha256:";
  if (provenance_name.size() < prefix.size() ||
      provenance_name.compare(0u, prefix.size(), prefix) != 0) {
    return false;
  }
  const std::string_view digest(provenance_name.c_str() + prefix.size(),
                                provenance_name.size() - prefix.size());
  return digest.size() >= 6u && digest.size() <= 64u &&
         std::all_of(digest.begin(), digest.end(), [](char c) {
           return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
         });
}

bool IsMetaprogrammingPropertyBehaviorObjectLike(
    const Objc3PropertyDecl &property) {
  return property.id_spelling || property.instancetype_spelling ||
         property.object_pointer_type_spelling ||
         property.type == ValueType::ObjCId ||
         property.type == ValueType::ObjCClass ||
         property.type == ValueType::ObjCInstancetype ||
         property.type == ValueType::ObjCObjectPtr;
}

}  // namespace

std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
BuildMetaprogrammingDerivedMethodBundles(const Objc3Program &program) {
  std::unordered_map<std::string, const Objc3ImplementationDecl *> implementations;
  implementations.reserve(program.implementations.size());
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      implementations.emplace(implementation_decl.name, &implementation_decl);
    }
  }

  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle> bundles;
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      continue;
    }
    const bool has_derive_marker =
        interface_decl.objc_derive_declared ||
        !interface_decl.objc_derive_name.empty();
    if (!has_derive_marker) {
      continue;
    }
    const auto impl_it = implementations.find(interface_decl.name);
    if (impl_it == implementations.end()) {
      continue;
    }
    const std::string derive_name =
        NormalizeMetaprogrammingDeriveNameForEmission(
            interface_decl.objc_derive_name);
    if (derive_name.empty()) {
      continue;
    }

    std::string selector;
    std::size_t parameter_count = 0u;
    if (derive_name == "Equality") {
      selector = "isEqual:";
      parameter_count = 1u;
    } else if (derive_name == "Hash") {
      selector = "hash";
    } else if (derive_name == "DebugDescription") {
      selector = "debugDescription";
    }
    const auto selector_exists = [&selector](const auto &methods) {
      return std::any_of(methods.begin(), methods.end(),
                         [&selector](const Objc3MethodDecl &method_decl) {
                           return !method_decl.is_class_method &&
                                  method_decl.selector == selector;
                         });
    };
    if (selector.empty() || selector_exists(interface_decl.methods) ||
        selector_exists(impl_it->second->methods)) {
      continue;
    }

    Objc3IRMetaprogrammingDerivedMethodBundle bundle;
    bundle.implementation_name = impl_it->second->name;
    bundle.declaration_owner_identity = impl_it->second->semantic_link_symbol;
    bundle.export_owner_identity =
        BuildMetaprogrammingRuntimeClassOwnerIdentity(interface_decl.name);
    bundle.derive_name = derive_name;
    bundle.selector = selector;
    bundle.emitted_symbol =
        BuildMetaprogrammingDerivedMethodFunctionSymbol(
            bundle.implementation_name, bundle.selector);
    bundle.parameter_count = parameter_count;
    bundle.line = interface_decl.line;
    bundle.column = interface_decl.column;
    bundles.push_back(std::move(bundle));
  }

  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingDerivedMethodBundle &lhs,
               const Objc3IRMetaprogrammingDerivedMethodBundle &rhs) {
              return std::tie(lhs.implementation_name, lhs.selector,
                              lhs.derive_name) <
                     std::tie(rhs.implementation_name, rhs.selector,
                              rhs.derive_name);
            });
  return bundles;
}

std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
BuildMetaprogrammingMacroArtifactBundles(const Objc3Program &program) {
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle> bundles;
  for (const auto &fn : program.functions) {
    const bool has_macro_marker =
        fn.objc_macro_declared || !fn.objc_macro_name.empty();
    const bool has_macro_package =
        fn.objc_macro_package_declared || !fn.objc_macro_package_name.empty();
    const bool has_macro_provenance =
        fn.objc_macro_provenance_declared ||
        !fn.objc_macro_provenance_name.empty();
    if (!has_macro_marker || !has_macro_package || !has_macro_provenance ||
        !fn.is_pure || fn.is_prototype || fn.async_declared ||
        fn.throws_declared ||
        !IsSupportedMetaprogrammingMacroPackageNameForEmission(
            fn.objc_macro_package_name) ||
        !IsSupportedMetaprogrammingMacroProvenanceNameForEmission(
            fn.objc_macro_provenance_name)) {
      continue;
    }
    Objc3IRMetaprogrammingMacroArtifactBundle bundle;
    bundle.function_name = fn.name;
    bundle.macro_name = fn.objc_macro_name;
    bundle.package_name = fn.objc_macro_package_name;
    bundle.provenance_name = fn.objc_macro_provenance_name;
    bundle.cache_key_name = fn.objc_macro_cache_key_name;
    bundle.sandbox_policy_name = fn.objc_macro_sandbox_name;
    bundle.emitted_symbol =
        BuildMetaprogrammingMacroArtifactSymbol(fn.name, fn.objc_macro_name);
    bundle.line = fn.line;
    bundle.column = fn.column;
    bundles.push_back(std::move(bundle));
  }
  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingMacroArtifactBundle &lhs,
               const Objc3IRMetaprogrammingMacroArtifactBundle &rhs) {
              return std::tie(lhs.function_name, lhs.macro_name,
                              lhs.provenance_name, lhs.cache_key_name,
                              lhs.sandbox_policy_name) <
                     std::tie(rhs.function_name, rhs.macro_name,
                              rhs.provenance_name, rhs.cache_key_name,
                              rhs.sandbox_policy_name);
            });
  return bundles;
}

std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
BuildMetaprogrammingPropertyBehaviorArtifactBundles(const Objc3Program &program) {
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle> bundles;
  const auto append_properties =
      [&bundles](const auto &properties, const std::string &owner_kind,
                 const std::string &owner_name,
                 const std::string &declaration_owner_identity,
                 const std::string &export_owner_identity) {
        for (const auto &property : properties) {
          if (!property.property_behavior_declared) {
            continue;
          }
          const std::string behavior_name =
              NormalizeMetaprogrammingPropertyBehaviorNameForEmission(
                  property.property_behavior_name);
          if (behavior_name.empty() ||
              !IsMetaprogrammingPropertyBehaviorObjectLike(property)) {
            continue;
          }
          if (behavior_name == "Observed" &&
              (owner_kind == "protocol" || property.is_readonly ||
               !property.effective_setter_available)) {
            continue;
          }
          if (behavior_name == "Projected" &&
              (!property.is_readonly || property.has_setter ||
               property.effective_setter_available)) {
            continue;
          }

          Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle bundle;
          bundle.owner_kind = owner_kind;
          bundle.owner_name = owner_name;
          bundle.declaration_owner_identity = declaration_owner_identity;
          bundle.export_owner_identity = export_owner_identity;
          bundle.property_name = property.name;
          bundle.behavior_name = behavior_name;
          bundle.binding_symbol = property.executable_synthesized_binding_symbol;
          if (bundle.binding_symbol.empty()) {
            bundle.binding_symbol = property.ivar_binding_symbol;
          }
          bundle.emitted_symbol =
              BuildMetaprogrammingPropertyBehaviorArtifactSymbol(
                  owner_kind, owner_name, property.name, behavior_name);
          bundle.line = property.line;
          bundle.column = property.column;
          bundles.push_back(std::move(bundle));
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    append_properties(
        interface_decl.properties,
        interface_decl.has_category ? "category-interface" : "class-interface",
        interface_decl.has_category
            ? BuildMetaprogrammingCategoryOwnerName(interface_decl.name,
                                                    interface_decl.category_name)
            : interface_decl.name,
        interface_decl.semantic_link_symbol,
        interface_decl.has_category
            ? BuildMetaprogrammingRuntimeCategoryOwnerIdentity(
                  interface_decl.name, interface_decl.category_name)
            : BuildMetaprogrammingRuntimeClassOwnerIdentity(interface_decl.name));
  }
  for (const auto &protocol_decl : program.protocols) {
    append_properties(protocol_decl.properties, "protocol", protocol_decl.name,
                      protocol_decl.semantic_link_symbol,
                      protocol_decl.semantic_link_symbol);
  }
  for (const auto &implementation_decl : program.implementations) {
    append_properties(
        implementation_decl.properties,
        implementation_decl.has_category ? "category-implementation"
                                         : "class-implementation",
        implementation_decl.has_category
            ? BuildMetaprogrammingCategoryOwnerName(
                  implementation_decl.name, implementation_decl.category_name)
            : implementation_decl.name,
        implementation_decl.semantic_link_symbol,
        implementation_decl.has_category
            ? BuildMetaprogrammingRuntimeCategoryOwnerIdentity(
                  implementation_decl.name,
                  implementation_decl.category_name)
            : BuildMetaprogrammingRuntimeClassOwnerIdentity(
                  implementation_decl.name));
  }

  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &lhs,
               const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &rhs) {
              return std::tie(lhs.owner_kind, lhs.owner_name,
                              lhs.property_name, lhs.behavior_name) <
                     std::tie(rhs.owner_kind, rhs.owner_name,
                              rhs.property_name, rhs.behavior_name);
            });
  return bundles;
}

}  // namespace objc3::artifacts::frontend
