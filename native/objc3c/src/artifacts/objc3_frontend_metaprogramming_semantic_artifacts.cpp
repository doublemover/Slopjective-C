#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

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

Objc3MetaprogrammingSynthesizedArtifactEmissionContract
BuildMetaprogrammingSynthesizedArtifactEmissionContract(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle> &derive_bundles,
    const std::vector<Objc3IRMetaprogrammingMacroArtifactBundle> &macro_bundles,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles) {
  Objc3MetaprogrammingSynthesizedArtifactEmissionContract contract;
  contract.derive_inventory_sites = dependency_contract.derive_inventory_sites;
  contract.emitted_derive_method_sites = derive_bundles.size();
  contract.emitted_macro_artifact_sites = macro_bundles.size();
  contract.emitted_property_behavior_artifact_sites =
      property_behavior_bundles.size();
  contract.emitted_global_artifact_sites =
      contract.emitted_derive_method_sites +
      contract.emitted_macro_artifact_sites +
      contract.emitted_property_behavior_artifact_sites;
  contract.emitted_runtime_method_list_sites =
      contract.emitted_derive_method_sites;
  contract.guard_blocked_sites = dependency_contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      dependency_contract.deterministic &&
      contract.emitted_derive_method_sites <=
          dependency_contract.derived_selector_artifact_sites &&
      contract.emitted_macro_artifact_sites <=
          dependency_contract.macro_replay_visible_sites &&
      contract.emitted_property_behavior_artifact_sites <=
          dependency_contract.property_behavior_sites;
  return contract;
}

Objc3MetaprogrammingExpansionLoweringContract
BuildMetaprogrammingExpansionLoweringContract(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary) {
  Objc3MetaprogrammingExpansionLoweringContract contract;
  contract.derive_inventory_sites =
      derive_summary.supported_derive_request_sites;
  contract.derived_selector_artifact_sites =
      derive_summary.generated_method_entry_count;
  contract.macro_replay_visible_sites =
      macro_summary.expansion_visible_macro_sites;
  contract.property_behavior_sites =
      property_legality_summary.property_behavior_sites;
  contract.synthesized_binding_sites =
      property_source_summary.synthesized_binding_visible_sites;
  contract.synthesized_getter_sites =
      property_source_summary.synthesized_getter_visible_sites;
  contract.synthesized_setter_sites =
      property_source_summary.synthesized_setter_visible_sites;
  contract.replay_visible_metadata_sites =
      contract.derived_selector_artifact_sites +
      contract.macro_replay_visible_sites + contract.property_behavior_sites +
      contract.synthesized_binding_sites + contract.synthesized_getter_sites +
      contract.synthesized_setter_sites;
  contract.guard_blocked_sites =
      derive_summary.unsupported_derive_request_sites +
      derive_summary.unsupported_topology_sites +
      derive_summary.selector_conflict_sites +
      macro_summary.incomplete_macro_metadata_sites +
      macro_summary.orphan_macro_metadata_sites +
      macro_summary.invalid_package_sites +
      macro_summary.invalid_provenance_sites +
      macro_summary.nondeterministic_callable_sites +
      macro_summary.unsupported_callable_topology_sites +
      property_legality_summary.unsupported_behavior_sites +
      property_legality_summary.observed_on_protocol_sites +
      property_legality_summary.observed_readonly_conflict_sites +
      property_legality_summary.projected_writable_conflict_sites +
      property_legality_summary.non_object_behavior_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      derive_summary.deterministic &&
      derive_summary.ready_for_lowering_and_runtime &&
      macro_summary.deterministic &&
      macro_summary.ready_for_lowering_and_runtime &&
      property_legality_summary.deterministic &&
      property_legality_summary.ready_for_lowering_and_runtime &&
      property_source_summary.deterministic_handoff &&
      property_source_summary.ready_for_semantic_expansion;
  return contract;
}

std::string BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"derive_marker_sites\":" << summary.derive_marker_sites
      << ",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"interface_property_behavior_sites\":"
      << summary.interface_property_behavior_sites
      << ",\"implementation_property_behavior_sites\":"
      << summary.implementation_property_behavior_sites
      << ",\"protocol_property_behavior_sites\":"
      << summary.protocol_property_behavior_sites
      << ",\"synthesized_binding_visible_sites\":"
      << summary.synthesized_binding_visible_sites
      << ",\"synthesized_getter_visible_sites\":"
      << summary.synthesized_getter_visible_sites
      << ",\"synthesized_setter_visible_sites\":"
      << summary.synthesized_setter_visible_sites
      << ",\"property_behavior_contract_violation_sites\":"
      << summary.property_behavior_contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"derive_macro_source_supported\":"
      << (summary.derive_macro_source_supported ? "true" : "false")
      << ",\"macro_package_provenance_surface_reused\":"
      << (summary.macro_package_provenance_surface_reused ? "true" : "false")
      << ",\"property_behavior_source_supported\":"
      << (summary.property_behavior_source_supported ? "true" : "false")
      << ",\"synthesized_visibility_surface_reused\":"
      << (summary.synthesized_visibility_surface_reused ? "true" : "false")
      << ",\"derive_synthesis_deferred\":"
      << (summary.derive_synthesis_deferred ? "true" : "false")
      << ",\"macro_execution_deferred\":"
      << (summary.macro_execution_deferred ? "true" : "false")
      << ",\"property_behavior_runtime_deferred\":"
      << (summary.property_behavior_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_core_implementation\":"
      << (summary.ready_for_core_implementation ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"derive_request_sites\":" << summary.derive_request_sites
      << ",\"supported_derive_request_sites\":"
      << summary.supported_derive_request_sites
      << ",\"unsupported_derive_request_sites\":"
      << summary.unsupported_derive_request_sites
      << ",\"unsupported_topology_sites\":"
      << summary.unsupported_topology_sites
      << ",\"equatable_alias_sites\":" << summary.equatable_alias_sites
      << ",\"equality_derive_sites\":" << summary.equality_derive_sites
      << ",\"hash_derive_sites\":" << summary.hash_derive_sites
      << ",\"debug_description_derive_sites\":"
      << summary.debug_description_derive_sites
      << ",\"selector_conflict_sites\":" << summary.selector_conflict_sites
      << ",\"generated_method_entry_count\":"
      << summary.generated_method_entry_count
      << ",\"expansion_inventory_rows_lexicographic\":"
      << BuildStringArrayJson(summary.expansion_inventory_rows_lexicographic)
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"supported_derive_inventory_landed\":"
      << (summary.supported_derive_inventory_landed ? "true" : "false")
      << ",\"unsupported_derive_fail_closed\":"
      << (summary.unsupported_derive_fail_closed ? "true" : "false")
      << ",\"unsupported_topology_fail_closed\":"
      << (summary.unsupported_topology_fail_closed ? "true" : "false")
      << ",\"selector_conflicts_fail_closed\":"
      << (summary.selector_conflicts_fail_closed ? "true" : "false")
      << ",\"runtime_materialization_deferred\":"
      << (summary.runtime_materialization_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"safe_macro_callable_sites\":"
      << summary.safe_macro_callable_sites
      << ",\"incomplete_macro_metadata_sites\":"
      << summary.incomplete_macro_metadata_sites
      << ",\"orphan_macro_metadata_sites\":"
      << summary.orphan_macro_metadata_sites
      << ",\"invalid_package_sites\":" << summary.invalid_package_sites
      << ",\"invalid_provenance_sites\":"
      << summary.invalid_provenance_sites
      << ",\"nondeterministic_callable_sites\":"
      << summary.nondeterministic_callable_sites
      << ",\"unsupported_callable_topology_sites\":"
      << summary.unsupported_callable_topology_sites
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"metadata_completeness_enforced\":"
      << (summary.metadata_completeness_enforced ? "true" : "false")
      << ",\"sandbox_namespace_enforced\":"
      << (summary.sandbox_namespace_enforced ? "true" : "false")
      << ",\"provenance_determinism_enforced\":"
      << (summary.provenance_determinism_enforced ? "true" : "false")
      << ",\"callable_determinism_enforced\":"
      << (summary.callable_determinism_enforced ? "true" : "false")
      << ",\"macro_execution_deferred\":"
      << (summary.macro_execution_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string
BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"supported_behavior_sites\":" << summary.supported_behavior_sites
      << ",\"unsupported_behavior_sites\":" << summary.unsupported_behavior_sites
      << ",\"observed_behavior_sites\":" << summary.observed_behavior_sites
      << ",\"projected_behavior_sites\":" << summary.projected_behavior_sites
      << ",\"observed_on_protocol_sites\":"
      << summary.observed_on_protocol_sites
      << ",\"observed_readonly_conflict_sites\":"
      << summary.observed_readonly_conflict_sites
      << ",\"projected_writable_conflict_sites\":"
      << summary.projected_writable_conflict_sites
      << ",\"non_object_behavior_sites\":" << summary.non_object_behavior_sites
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"supported_behavior_inventory_landed\":"
      << (summary.supported_behavior_inventory_landed ? "true" : "false")
      << ",\"unsupported_behavior_fail_closed\":"
      << (summary.unsupported_behavior_fail_closed ? "true" : "false")
      << ",\"owner_topology_fail_closed\":"
      << (summary.owner_topology_fail_closed ? "true" : "false")
      << ",\"interaction_legality_fail_closed\":"
      << (summary.interaction_legality_fail_closed ? "true" : "false")
      << ",\"storage_legality_fail_closed\":"
      << (summary.storage_legality_fail_closed ? "true" : "false")
      << ",\"runtime_materialization_deferred\":"
      << (summary.runtime_materialization_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingExpansionLoweringContractJson(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary,
    const Objc3MetaprogrammingExpansionLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3MetaprogrammingExpansionLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringSurfacePath)
      << "\",\"property_source_contract_id\":\""
      << EscapeJsonString(property_source_summary.contract_id)
      << "\",\"derive_contract_id\":\""
      << EscapeJsonString(derive_summary.contract_id)
      << "\",\"macro_contract_id\":\""
      << EscapeJsonString(macro_summary.contract_id)
      << "\",\"property_legality_contract_id\":\""
      << EscapeJsonString(property_legality_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"derive_inventory_sites\":" << contract.derive_inventory_sites
      << ",\"derived_selector_artifact_sites\":"
      << contract.derived_selector_artifact_sites
      << ",\"macro_replay_visible_sites\":"
      << contract.macro_replay_visible_sites
      << ",\"property_behavior_sites\":" << contract.property_behavior_sites
      << ",\"synthesized_binding_sites\":"
      << contract.synthesized_binding_sites
      << ",\"synthesized_getter_sites\":"
      << contract.synthesized_getter_sites
      << ",\"synthesized_setter_sites\":"
      << contract.synthesized_setter_sites
      << ",\"replay_visible_metadata_sites\":"
      << contract.replay_visible_metadata_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionSurfacePath)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringContractId)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract)
      << "\",\"emission_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingSynthesizedArtifactEmissionModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionDeferredModel)
      << "\",\"dependency_replay_key\":\""
      << EscapeJsonString(Objc3MetaprogrammingExpansionLoweringReplayKey(
             dependency_contract))
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"derive_inventory_sites\":" << contract.derive_inventory_sites
      << ",\"emitted_derive_method_sites\":"
      << contract.emitted_derive_method_sites
      << ",\"emitted_macro_artifact_sites\":"
      << contract.emitted_macro_artifact_sites
      << ",\"emitted_property_behavior_artifact_sites\":"
      << contract.emitted_property_behavior_artifact_sites
      << ",\"emitted_global_artifact_sites\":"
      << contract.emitted_global_artifact_sites
      << ",\"emitted_runtime_method_list_sites\":"
      << contract.emitted_runtime_method_list_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::size_t CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle> &bundles,
    std::string_view owner_kind) {
  return static_cast<std::size_t>(std::count_if(
      bundles.begin(), bundles.end(),
      [owner_kind](const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &bundle) {
        return bundle.owner_kind == owner_kind;
      }));
}

Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
BuildMetaprogrammingModuleInterfaceReplayPreservationSummary(
    const Objc3MetaprogrammingExpansionLoweringContract &expansion_contract,
    const std::string &expansion_replay_key,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
        &synthesized_contract,
    const std::string &synthesized_replay_key,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary summary;
  summary.expansion_lowering_replay_key = expansion_replay_key;
  summary.synthesized_emission_replay_key = synthesized_replay_key;
  summary.local_derive_method_count =
      synthesized_contract.emitted_derive_method_sites;
  summary.local_macro_artifact_count =
      synthesized_contract.emitted_macro_artifact_sites;
  summary.local_interface_property_behavior_artifact_count =
      CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
          property_behavior_bundles, "class-interface");
  summary.local_implementation_property_behavior_artifact_count =
      CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
          property_behavior_bundles, "class-implementation");
  summary.local_runtime_method_list_count =
      synthesized_contract.emitted_runtime_method_list_sites;
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      IsValidObjc3MetaprogrammingExpansionLoweringContract(expansion_contract) &&
      IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
          synthesized_contract);
  summary.deterministic =
      expansion_contract.deterministic && synthesized_contract.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.metaprogramming_module_interface_replay_preservation_present) {
      continue;
    }
    ++summary.imported_module_count;
    if (!surface.frontend_closure_summary.module_name.empty()) {
      summary.imported_module_names_lexicographic.push_back(
          surface.frontend_closure_summary.module_name);
    }
    summary.imported_derive_method_count +=
        surface.metaprogramming_local_derive_method_count;
    summary.imported_macro_artifact_count +=
        surface.metaprogramming_local_macro_artifact_count;
    summary.imported_interface_property_behavior_artifact_count +=
        surface.metaprogramming_local_interface_property_behavior_artifact_count;
    summary.imported_implementation_property_behavior_artifact_count +=
        surface.metaprogramming_local_implementation_property_behavior_artifact_count;
    summary.imported_runtime_method_list_count +=
        surface.metaprogramming_local_runtime_method_list_count;
    summary.deterministic =
        summary.deterministic && surface.metaprogramming_deterministic;
  }
  std::sort(summary.imported_module_names_lexicographic.begin(),
            summary.imported_module_names_lexicographic.end());
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready &&
      summary.imported_module_names_lexicographic.size() ==
          summary.imported_module_count;
  std::ostringstream replay_key;
  replay_key << Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary()
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";expansion_lowering_replay_key=" << expansion_replay_key
             << ";synthesized_emission_replay_key=" << synthesized_replay_key
             << ";local_derive_method_count="
             << summary.local_derive_method_count
             << ";local_macro_artifact_count="
             << summary.local_macro_artifact_count
             << ";local_interface_property_behavior_artifact_count="
             << summary.local_interface_property_behavior_artifact_count
             << ";local_implementation_property_behavior_artifact_count="
             << summary.local_implementation_property_behavior_artifact_count
             << ";local_runtime_method_list_count="
             << summary.local_runtime_method_list_count
             << ";imported_derive_method_count="
             << summary.imported_derive_method_count
             << ";imported_macro_artifact_count="
             << summary.imported_macro_artifact_count
             << ";imported_interface_property_behavior_artifact_count="
             << summary.imported_interface_property_behavior_artifact_count
             << ";imported_implementation_property_behavior_artifact_count="
             << summary.imported_implementation_property_behavior_artifact_count
             << ";imported_runtime_method_list_count="
             << summary.imported_runtime_method_list_count;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"expansion_lowering_replay_key\":\""
      << EscapeJsonString(summary.expansion_lowering_replay_key)
      << "\",\"synthesized_emission_replay_key\":\""
      << EscapeJsonString(summary.synthesized_emission_replay_key)
      << "\",\"imported_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.imported_module_names_lexicographic)
      << ",\"local_derive_method_count\":"
      << summary.local_derive_method_count
      << ",\"local_macro_artifact_count\":"
      << summary.local_macro_artifact_count
      << ",\"local_interface_property_behavior_artifact_count\":"
      << summary.local_interface_property_behavior_artifact_count
      << ",\"local_implementation_property_behavior_artifact_count\":"
      << summary.local_implementation_property_behavior_artifact_count
      << ",\"local_runtime_method_list_count\":"
      << summary.local_runtime_method_list_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_derive_method_count\":"
      << summary.imported_derive_method_count
      << ",\"imported_macro_artifact_count\":"
      << summary.imported_macro_artifact_count
      << ",\"imported_interface_property_behavior_artifact_count\":"
      << summary.imported_interface_property_behavior_artifact_count
      << ",\"imported_implementation_property_behavior_artifact_count\":"
      << summary.imported_implementation_property_behavior_artifact_count
      << ",\"imported_runtime_method_list_count\":"
      << summary.imported_runtime_method_list_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary
BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &module_interface_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    const Objc3FrontendOptions &options) {
  Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary summary;
  if (!options.metaprogramming_cache_root_relative_path.empty()) {
    summary.cache_root_relative_path =
        options.metaprogramming_cache_root_relative_path;
  }
  summary.metaprogramming_replay_key = module_interface_summary.replay_key;
  summary.local_macro_artifact_count =
      module_interface_summary.local_macro_artifact_count;
  summary.local_property_behavior_artifact_count =
      module_interface_summary.local_interface_property_behavior_artifact_count +
      module_interface_summary
          .local_implementation_property_behavior_artifact_count;
  summary.runtime_import_artifact_ready =
      module_interface_summary.runtime_import_artifact_ready;
  summary.deterministic = module_interface_summary.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.metaprogramming_macro_host_process_cache_runtime_integration_present) {
      continue;
    }
    ++summary.imported_module_count;
    summary.deterministic =
        summary.deterministic &&
        surface.metaprogramming_macro_host_process_cache_deterministic;
  }
  summary.separate_compilation_ready =
      summary.runtime_import_artifact_ready &&
      module_interface_summary.separate_compilation_preservation_ready;
  std::ostringstream replay_key;
  replay_key << Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary()
             << ";metaprogramming_replay_key=" << module_interface_summary.replay_key
             << ";local_macro_artifact_count="
             << summary.local_macro_artifact_count
             << ";local_property_behavior_artifact_count="
             << summary.local_property_behavior_artifact_count
             << ";imported_module_count=" << summary.imported_module_count
             << ";cache_root_relative_path=" << summary.cache_root_relative_path
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_ready="
             << (summary.separate_compilation_ready ? "true" : "false")
             << ";deterministic="
             << (summary.deterministic ? "true" : "false");
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson(
    const Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"host_executable_relative_path\":\""
      << EscapeJsonString(summary.host_executable_relative_path)
      << "\",\"cache_root_relative_path\":\""
      << EscapeJsonString(summary.cache_root_relative_path)
      << "\",\"host_model\":\"" << EscapeJsonString(summary.host_model)
      << "\",\"toolchain_model\":\""
      << EscapeJsonString(summary.toolchain_model)
      << "\",\"cache_model\":\"" << EscapeJsonString(summary.cache_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"metaprogramming_replay_key\":\""
      << EscapeJsonString(summary.metaprogramming_replay_key)
      << "\",\"local_macro_artifact_count\":"
      << summary.local_macro_artifact_count
      << ",\"local_property_behavior_artifact_count\":"
      << summary.local_property_behavior_artifact_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_ready\":"
      << (summary.separate_compilation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
