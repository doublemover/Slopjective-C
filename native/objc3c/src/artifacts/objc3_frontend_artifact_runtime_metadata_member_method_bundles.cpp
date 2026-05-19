#include "artifacts/objc3_frontend_artifact_runtime_metadata_typed_bundles.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace objc3::artifacts::frontend {
namespace {

std::string DetermineRuntimeMetadataMethodOwnerFamilyKind(
    const std::string &owner_kind) {
  if (owner_kind == "protocol") {
    return "protocol";
  }
  if (owner_kind == "class-interface" ||
      owner_kind == "class-implementation") {
    return "class";
  }
  if (owner_kind == "category-interface" ||
      owner_kind == "category-implementation") {
    return "category";
  }
  return {};
}

std::string BuildRuntimeMetadataInstanceMethodOwnerIdentity(
    const std::string &declaration_owner_identity,
    const std::string &selector) {
  return declaration_owner_identity + "::instance_method:" + selector;
}

}  // namespace

bool BuildObjc3FrontendRuntimeMetadataMethodListBundles(
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
        &derived_method_bundles,
    const std::vector<Objc3IRRuntimeMetadataPropertyBundle> &property_bundles,
    std::vector<Objc3IRRuntimeMetadataMethodListBundle>
        &method_list_bundles) {
  method_list_bundles.clear();
  method_list_bundles.reserve(source_graph.method_nodes_lexicographic.size());

  std::unordered_map<std::string, std::size_t> method_bundle_indexes;
  method_bundle_indexes.reserve(source_graph.method_nodes_lexicographic.size());
  std::unordered_set<std::string> method_owner_identities;
  method_owner_identities.reserve(source_graph.method_nodes_lexicographic.size());
  for (const auto &method_node : source_graph.method_nodes_lexicographic) {
    const std::string list_kind =
        method_node.is_class_method ? "class" : "instance";
    const std::string owner_family_kind =
        DetermineRuntimeMetadataMethodOwnerFamilyKind(method_node.owner_kind);
    if (method_node.owner_kind.empty() || method_node.owner_name.empty() ||
        owner_family_kind.empty() || method_node.owner_identity.empty() ||
        method_node.declaration_owner_identity.empty() ||
        method_node.export_owner_identity.empty() ||
        method_node.selector.empty() || method_node.return_type_name.empty()) {
      return false;
    }

    const std::string bundle_key =
        method_node.declaration_owner_identity + "|" + list_kind;
    auto bundle_it = method_bundle_indexes.find(bundle_key);
    if (bundle_it == method_bundle_indexes.end()) {
      Objc3IRRuntimeMetadataMethodListBundle bundle;
      bundle.owner_kind = method_node.owner_kind;
      bundle.owner_name = method_node.owner_name;
      bundle.owner_family_kind = owner_family_kind;
      bundle.declaration_owner_identity =
          method_node.declaration_owner_identity;
      bundle.export_owner_identity = method_node.export_owner_identity;
      bundle.list_kind = list_kind;
      method_list_bundles.push_back(std::move(bundle));
      bundle_it =
          method_bundle_indexes
              .emplace(bundle_key, method_list_bundles.size() - 1u)
              .first;
    }

    auto &bundle = method_list_bundles[bundle_it->second];
    if (bundle.owner_kind != method_node.owner_kind ||
        bundle.owner_name != method_node.owner_name ||
        bundle.owner_family_kind != owner_family_kind ||
        bundle.export_owner_identity != method_node.export_owner_identity ||
        bundle.list_kind != list_kind) {
      return false;
    }

    Objc3IRRuntimeMetadataMethodEntry entry;
    entry.owner_identity = method_node.owner_identity;
    entry.selector = method_node.selector;
    entry.return_type_name = method_node.return_type_name;
    entry.parameter_count = method_node.parameter_count;
    entry.has_body = method_node.has_body;
    entry.effective_direct_dispatch = method_node.effective_direct_dispatch;
    entry.objc_final_declared = method_node.objc_final_declared;
    bundle.entries_lexicographic.push_back(std::move(entry));
    method_owner_identities.insert(method_node.owner_identity);
  }

  for (const auto &property_bundle : property_bundles) {
    if (!property_bundle.synthesizes_executable_accessors ||
        property_bundle.owner_name.empty() ||
        property_bundle.declaration_owner_identity.empty() ||
        property_bundle.export_owner_identity.empty() ||
        property_bundle.property_name.empty() ||
        property_bundle.type_name.empty() ||
        property_bundle.executable_synthesized_binding_kind.empty() ||
        property_bundle.executable_synthesized_binding_symbol.empty()) {
      continue;
    }

    const std::string owner_family_kind =
        DetermineRuntimeMetadataMethodOwnerFamilyKind(
            property_bundle.owner_kind);
    if (owner_family_kind.empty()) {
      return false;
    }

    const auto append_synthesized_accessor =
        [&](const std::string &selector, const std::string &return_type_name,
            std::size_t parameter_count) {
          if (selector.empty() || return_type_name.empty()) {
            return false;
          }
          const std::string method_owner_identity =
              BuildRuntimeMetadataInstanceMethodOwnerIdentity(
                  property_bundle.declaration_owner_identity, selector);
          if (!method_owner_identities.insert(method_owner_identity).second) {
            return true;
          }

          const std::string bundle_key =
              property_bundle.declaration_owner_identity + "|instance";
          auto bundle_it = method_bundle_indexes.find(bundle_key);
          if (bundle_it == method_bundle_indexes.end()) {
            Objc3IRRuntimeMetadataMethodListBundle bundle;
            bundle.owner_kind = property_bundle.owner_kind;
            bundle.owner_name = property_bundle.owner_name;
            bundle.owner_family_kind = owner_family_kind;
            bundle.declaration_owner_identity =
                property_bundle.declaration_owner_identity;
            bundle.export_owner_identity =
                property_bundle.export_owner_identity;
            bundle.list_kind = "instance";
            method_list_bundles.push_back(std::move(bundle));
            bundle_it =
                method_bundle_indexes
                    .emplace(bundle_key, method_list_bundles.size() - 1u)
                    .first;
          }

          auto &bundle = method_list_bundles[bundle_it->second];
          if (bundle.owner_kind != property_bundle.owner_kind ||
              bundle.owner_name != property_bundle.owner_name ||
              bundle.owner_family_kind != owner_family_kind ||
              bundle.export_owner_identity !=
                  property_bundle.export_owner_identity ||
              bundle.list_kind != "instance") {
            return false;
          }

          Objc3IRRuntimeMetadataMethodEntry entry;
          entry.owner_identity = method_owner_identity;
          entry.selector = selector;
          entry.return_type_name = return_type_name;
          entry.parameter_count = parameter_count;
          entry.has_body = true;
          entry.effective_direct_dispatch = false;
          entry.objc_final_declared = false;
          bundle.entries_lexicographic.push_back(std::move(entry));
          return true;
        };

    if (!append_synthesized_accessor(property_bundle.effective_getter_selector,
                                     property_bundle.type_name, 0u)) {
      return false;
    }
    if (property_bundle.effective_setter_available &&
        !append_synthesized_accessor(property_bundle.effective_setter_selector,
                                     "void", 1u)) {
      return false;
    }
  }

  for (const auto &derive_bundle : derived_method_bundles) {
    if (derive_bundle.implementation_name.empty() ||
        derive_bundle.declaration_owner_identity.empty() ||
        derive_bundle.export_owner_identity.empty() ||
        derive_bundle.selector.empty() || derive_bundle.emitted_symbol.empty()) {
      return false;
    }
    const std::string method_owner_identity =
        BuildRuntimeMetadataInstanceMethodOwnerIdentity(
            derive_bundle.declaration_owner_identity, derive_bundle.selector);
    if (!method_owner_identities.insert(method_owner_identity).second) {
      continue;
    }
    const std::string bundle_key =
        derive_bundle.declaration_owner_identity + "|instance";
    auto bundle_it = method_bundle_indexes.find(bundle_key);
    if (bundle_it == method_bundle_indexes.end()) {
      Objc3IRRuntimeMetadataMethodListBundle bundle;
      bundle.owner_kind = "class-implementation";
      bundle.owner_name = derive_bundle.implementation_name;
      bundle.owner_family_kind = "class";
      bundle.declaration_owner_identity =
          derive_bundle.declaration_owner_identity;
      bundle.export_owner_identity = derive_bundle.export_owner_identity;
      bundle.list_kind = "instance";
      method_list_bundles.push_back(std::move(bundle));
      bundle_it =
          method_bundle_indexes
              .emplace(bundle_key, method_list_bundles.size() - 1u)
              .first;
    }

    auto &bundle = method_list_bundles[bundle_it->second];
    if (bundle.owner_kind != "class-implementation" ||
        bundle.owner_name != derive_bundle.implementation_name ||
        bundle.owner_family_kind != "class" ||
        bundle.export_owner_identity != derive_bundle.export_owner_identity ||
        bundle.list_kind != "instance") {
      return false;
    }

    Objc3IRRuntimeMetadataMethodEntry entry;
    entry.owner_identity = method_owner_identity;
    entry.selector = derive_bundle.selector;
    entry.return_type_name = "i32";
    entry.parameter_count = derive_bundle.parameter_count;
    entry.has_body = true;
    entry.effective_direct_dispatch = false;
    entry.objc_final_declared = false;
    bundle.entries_lexicographic.push_back(std::move(entry));
  }

  std::sort(
      method_list_bundles.begin(), method_list_bundles.end(),
      [](const Objc3IRRuntimeMetadataMethodListBundle &lhs,
         const Objc3IRRuntimeMetadataMethodListBundle &rhs) {
        return std::tie(lhs.owner_family_kind, lhs.declaration_owner_identity,
                        lhs.list_kind) <
               std::tie(rhs.owner_family_kind, rhs.declaration_owner_identity,
                        rhs.list_kind);
      });
  for (auto &bundle : method_list_bundles) {
    std::sort(bundle.entries_lexicographic.begin(),
              bundle.entries_lexicographic.end(),
              [](const Objc3IRRuntimeMetadataMethodEntry &lhs,
                 const Objc3IRRuntimeMetadataMethodEntry &rhs) {
                return std::tie(lhs.selector, lhs.owner_identity,
                                lhs.parameter_count, lhs.return_type_name,
                                lhs.has_body, lhs.effective_direct_dispatch,
                                lhs.objc_final_declared) <
                       std::tie(rhs.selector, rhs.owner_identity,
                                rhs.parameter_count, rhs.return_type_name,
                                rhs.has_body, rhs.effective_direct_dispatch,
                                rhs.objc_final_declared);
              });
  }
  return true;
}

}  // namespace objc3::artifacts::frontend
