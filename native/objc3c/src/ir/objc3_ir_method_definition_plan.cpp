#include "ir/objc3_ir_method_definition_plan.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_symbol_model.h"
#include "ir/objc3_ir_type_model.h"

bool Objc3IRRuntimeMetadataPropertyBundleIsImplementationOwned(
    const Objc3IRRuntimeMetadataPropertyBundle &bundle) {
  return bundle.synthesizes_executable_accessors;
}

Objc3IRMethodDefinitionPlan BuildObjc3IRMethodDefinitionPlan(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata) {
  Objc3IRMethodDefinitionPlan plan;
  std::unordered_map<std::string, std::string> implementation_superclass_names;
  std::unordered_map<std::string, bool> interface_direct_members_by_name;
  const auto record_runtime_dispatch_return_type =
      [&plan](const std::string &owner_name, const std::string &selector,
              bool is_class_method, ValueType return_type) {
        const std::string key =
            BuildDirectDispatchMethodKey(owner_name, selector, is_class_method);
        const auto existing =
            plan.runtime_dispatch_return_types_by_key.find(key);
        if (existing != plan.runtime_dispatch_return_types_by_key.end()) {
          if (existing->second != return_type) {
            plan.error =
                "conflicting runtime dispatch return type for method '" +
                owner_name + " " + selector + "'";
            return false;
          }
          return true;
        }
        plan.runtime_dispatch_return_types_by_key.emplace(key, return_type);
        return true;
      };
  const auto record_method_return_type =
      [&record_runtime_dispatch_return_type](const std::string &owner_name,
                                             const Objc3MethodDecl &method) {
        return record_runtime_dispatch_return_type(
            owner_name, method.selector, method.is_class_method,
            method.return_type);
      };
  const auto build_method_direct_dispatch_signature =
      [](const Objc3MethodDecl &method) {
        Objc3IRDirectDispatchSignature signature;
        signature.return_type = method.return_type;
        signature.param_types.reserve(method.params.size());
        for (const FuncParam &param : method.params) {
          signature.param_types.push_back(param.type);
        }
        return signature;
      };
  const auto record_direct_dispatch =
      [&plan](const std::string &owner_name, const std::string &selector,
              bool is_class_method, const std::string &symbol,
              const Objc3IRDirectDispatchSignature &signature) {
        const std::string key =
            BuildDirectDispatchMethodKey(owner_name, selector, is_class_method);
        const auto existing_symbol =
            plan.direct_dispatch_symbols_by_key.find(key);
        if (existing_symbol != plan.direct_dispatch_symbols_by_key.end() &&
            existing_symbol->second != symbol) {
          plan.error = "conflicting direct dispatch symbol for method '" +
                       owner_name + " " + selector + "'";
          return false;
        }
        const auto existing_signature =
            plan.direct_dispatch_signatures_by_key.find(key);
        if (existing_signature != plan.direct_dispatch_signatures_by_key.end() &&
            (existing_signature->second.return_type != signature.return_type ||
             existing_signature->second.param_types != signature.param_types)) {
          plan.error = "conflicting direct dispatch signature for method '" +
                       owner_name + " " + selector + "'";
          return false;
        }
        plan.direct_dispatch_symbols_by_key.emplace(key, symbol);
        plan.direct_dispatch_signatures_by_key.emplace(key, signature);
        return true;
      };
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      for (const auto &method : interface_decl.methods) {
        if (!record_method_return_type(interface_decl.name, method)) {
          return plan;
        }
      }
      continue;
    }
    implementation_superclass_names.emplace(interface_decl.name,
                                            interface_decl.super_name);
    plan.runtime_dispatch_superclass_by_name.emplace(interface_decl.name,
                                                     interface_decl.super_name);
    interface_direct_members_by_name.emplace(
        interface_decl.name, interface_decl.objc_direct_members_declared);
    for (const auto &method : interface_decl.methods) {
      if (!record_method_return_type(interface_decl.name, method)) {
        return plan;
      }
    }
  }

  std::unordered_map<std::string, std::size_t> method_symbol_counts;
  std::unordered_set<std::string> method_owner_identities;
  for (const auto &implementation : program.implementations) {
    const auto direct_members_it =
        interface_direct_members_by_name.find(implementation.name);
    const bool direct_members_declared =
        !implementation.has_category &&
        direct_members_it != interface_direct_members_by_name.end() &&
        direct_members_it->second;
    for (const auto &method : implementation.methods) {
      if (!record_method_return_type(implementation.name, method)) {
        return plan;
      }
      if (!method.has_body) {
        continue;
      }
      const std::string base_symbol = BuildImplementationMethodFunctionSymbol(
          implementation.name, method.selector, method.is_class_method);
      const std::size_t ordinal = method_symbol_counts[base_symbol]++;
      const std::string symbol =
          ordinal == 0 ? base_symbol
                       : base_symbol + "_" + std::to_string(ordinal + 1u);
      const auto super_it =
          implementation_superclass_names.find(implementation.name);
      const std::string superclass_name =
          super_it == implementation_superclass_names.end() ? std::string()
                                                            : super_it->second;
      plan.method_definitions.push_back(Objc3IRMethodDefinition{
          symbol,
          implementation.name,
          method.scope_path_symbol,
          superclass_name,
          &method,
          Objc3IRSyntheticMethodKind::None,
          ValueType::Unknown,
          0u,
          std::string{},
          std::string{},
          std::string{}});
      const bool effective_direct_dispatch =
          !implementation.has_category &&
          (method.objc_direct_declared ||
           (direct_members_declared && !method.objc_dynamic_declared));
      if (effective_direct_dispatch) {
        if (!record_direct_dispatch(
                implementation.name, method.selector, method.is_class_method,
                "@" + symbol,
                build_method_direct_dispatch_signature(method))) {
          return plan;
        }
      }
      if (!method.scope_path_symbol.empty()) {
        method_owner_identities.insert(method.scope_path_symbol);
      }
    }
  }

  for (const auto &bundle :
       frontend_metadata.runtime_metadata_property_bundles_lexicographic) {
    if (!Objc3IRRuntimeMetadataPropertyBundleIsImplementationOwned(bundle) ||
        bundle.declaration_owner_identity.empty() || bundle.owner_name.empty() ||
        bundle.type_name.empty() || bundle.effective_getter_selector.empty() ||
        bundle.executable_synthesized_binding_kind.empty() ||
        bundle.executable_synthesized_binding_symbol.empty()) {
      continue;
    }
    const ValueType property_type = RuntimeMetadataValueType(bundle.type_name);
    if (property_type == ValueType::Unknown ||
        property_type == ValueType::Void) {
      plan.error = "unsupported synthesized property accessor type '" +
                   bundle.type_name + "' for owner '" +
                   bundle.declaration_owner_identity + "'";
      return plan;
    }

    const auto append_synthesized_method =
        [&](const std::string &selector, Objc3IRSyntheticMethodKind kind) {
          if (selector.empty()) {
            plan.error =
                "missing synthesized property accessor selector for owner '" +
                bundle.declaration_owner_identity + "'";
            return;
          }
          const std::string method_owner_identity =
              BuildSynthesizedInstanceMethodOwnerIdentity(
                  bundle.declaration_owner_identity, selector);
          if (!method_owner_identities.insert(method_owner_identity).second) {
            return;
          }
          const std::string base_symbol =
              BuildImplementationMethodFunctionSymbol(bundle.owner_name,
                                                      selector, false);
          const std::size_t ordinal = method_symbol_counts[base_symbol]++;
          const std::string symbol =
              ordinal == 0 ? base_symbol
                           : base_symbol + "_" +
                                 std::to_string(ordinal + 1u);
          plan.method_definitions.push_back(Objc3IRMethodDefinition{
              symbol,
              bundle.owner_name,
              method_owner_identity,
              std::string{},
              nullptr,
              kind,
              property_type,
              kind == Objc3IRSyntheticMethodKind::PropertySetter ? 1u : 0u,
              bundle.ownership_lifetime_profile,
              bundle.ownership_runtime_hook_profile,
              bundle.accessor_ownership_profile,
          });
          if (!record_runtime_dispatch_return_type(
                  bundle.owner_name, selector, false,
                  kind == Objc3IRSyntheticMethodKind::PropertySetter
                      ? ValueType::Void
                      : property_type)) {
            return;
          }
          ++plan.synthesized_property_accessor_count;
        };

    append_synthesized_method(bundle.effective_getter_selector,
                              Objc3IRSyntheticMethodKind::PropertyGetter);
    if (!plan.error.empty()) {
      return plan;
    }
    if (bundle.effective_setter_available) {
      append_synthesized_method(bundle.effective_setter_selector,
                                Objc3IRSyntheticMethodKind::PropertySetter);
      if (!plan.error.empty()) {
        return plan;
      }
    }
  }

  for (const auto &bundle :
       frontend_metadata.metaprogramming_derived_method_bundles_lexicographic) {
    if (bundle.implementation_name.empty() ||
        bundle.declaration_owner_identity.empty() || bundle.selector.empty() ||
        bundle.emitted_symbol.empty()) {
      plan.error = "incomplete Part 10 derived method lowering bundle";
      return plan;
    }
    const std::string method_owner_identity =
        BuildSynthesizedInstanceMethodOwnerIdentity(
            bundle.declaration_owner_identity, bundle.selector);
    if (!method_owner_identities.insert(method_owner_identity).second) {
      continue;
    }
    Objc3IRSyntheticMethodKind synthetic_kind =
        Objc3IRSyntheticMethodKind::None;
    if (bundle.derive_name == "Equality") {
      synthetic_kind = Objc3IRSyntheticMethodKind::MetaprogrammingDerivedEquality;
    } else if (bundle.derive_name == "Hash") {
      synthetic_kind = Objc3IRSyntheticMethodKind::MetaprogrammingDerivedHash;
    } else if (bundle.derive_name == "DebugDescription") {
      synthetic_kind =
          Objc3IRSyntheticMethodKind::MetaprogrammingDerivedDebugDescription;
    } else {
      plan.error = "unsupported Part 10 derive kind '" +
                   bundle.derive_name + "'";
      return plan;
    }
    plan.method_definitions.push_back(Objc3IRMethodDefinition{
        bundle.emitted_symbol,
        bundle.implementation_name,
        method_owner_identity,
        std::string{},
        nullptr,
        synthetic_kind,
        ValueType::I32,
        bundle.parameter_count,
        std::string{},
        std::string{},
        std::string{},
    });
    ++plan.metaprogramming_derived_method_count;
    plan.metaprogramming_global_artifacts.push_back(
        Objc3IRMetaprogrammingGlobalArtifact{
            BuildSynthesizedPropertyStorageSymbol(bundle.emitted_symbol +
                                                  "_selector"),
            "derive=" + bundle.derive_name + ";owner=" +
                bundle.implementation_name + ";selector=" + bundle.selector +
                ";symbol=" + bundle.emitted_symbol});
  }

  for (const auto &bundle :
       frontend_metadata.metaprogramming_macro_artifact_bundles_lexicographic) {
    if (bundle.emitted_symbol.empty()) {
      plan.error = "incomplete Part 10 macro artifact lowering bundle";
      return plan;
    }
    plan.metaprogramming_global_artifacts.push_back(
        Objc3IRMetaprogrammingGlobalArtifact{
            bundle.emitted_symbol,
            "function=" + bundle.function_name + ";macro=" +
                bundle.macro_name + ";package=" + bundle.package_name +
                ";provenance=" + bundle.provenance_name + ";cache_key=" +
                bundle.cache_key_name + ";sandbox_policy=" +
                bundle.sandbox_policy_name});
  }

  for (const auto &bundle :
       frontend_metadata
           .metaprogramming_property_behavior_artifact_bundles_lexicographic) {
    if (bundle.emitted_symbol.empty()) {
      plan.error = "incomplete Part 10 property behavior lowering bundle";
      return plan;
    }
    plan.metaprogramming_global_artifacts.push_back(
        Objc3IRMetaprogrammingGlobalArtifact{
            bundle.emitted_symbol,
            "owner_kind=" + bundle.owner_kind + ";owner=" +
                bundle.owner_name + ";property=" + bundle.property_name +
                ";behavior=" + bundle.behavior_name + ";binding=" +
                bundle.binding_symbol});
  }

  return plan;
}
