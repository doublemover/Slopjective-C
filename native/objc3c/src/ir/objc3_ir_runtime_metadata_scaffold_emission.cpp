#include "ir/objc3_ir_runtime_metadata_scaffold_emission.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_artifact_emission.h"
#include "ir/objc3_ir_runtime_member_metadata_emission.h"
#include "ir/objc3_ir_runtime_object_metadata_emission.h"
#include "ir/objc3_ir_runtime_metadata_scaffold_comment_surfaces.h"
#include "lower/contracts/runtime_metadata_layout_ordering_policy_contracts.h"
#include "lower/metadata/runtime_metadata_layout_policy.h"

namespace {

std::string BuildMethodListKey(const std::string &owner_family_kind,
                               const std::string &owner_identity,
                               const std::string &list_kind) {
  return owner_family_kind + "|" + owner_identity + "|" + list_kind;
}

void EmitGenericDescriptorSection(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(family.descriptor_count);
  for (std::size_t i = 0; i < family.descriptor_count; ++i) {
    const std::string descriptor_symbol =
        BuildObjc3IRRuntimeMetadataDescriptorSymbol(
            layout_policy.descriptor_symbol_prefix, family.kind, i);
    descriptor_symbols.push_back(descriptor_symbol);
    out << descriptor_symbol << " = " << layout_policy.descriptor_linkage
        << " global [1 x i8] zeroinitializer, section \""
        << family.emitted_section_name << "\", align 1\n";
    retained_globals.push_back(descriptor_symbol);
  }

  const std::string aggregate_symbol = "@" + family.aggregate_symbol_name;
  out << aggregate_symbol << " = " << layout_policy.aggregate_linkage
      << (descriptor_symbols.empty() ? " constant " : " global ");
  if (descriptor_symbols.empty()) {
    out << "{ i64 } { i64 0 }";
  } else {
    out << "{ i64, [" << descriptor_symbols.size()
        << " x ptr] } { i64 " << descriptor_symbols.size() << ", ["
        << descriptor_symbols.size() << " x ptr] [";
    for (std::size_t i = 0; i < descriptor_symbols.size(); ++i) {
      if (i != 0) {
        out << ", ";
      }
      out << "ptr " << descriptor_symbols[i];
    }
    out << "] }";
  }
  out << ", section \"" << family.emitted_section_name << "\", align 8\n";
  retained_globals.push_back(aggregate_symbol);
}

bool BindImplementationMethodSymbols(
    const std::vector<Objc3IRMethodDefinition> &method_definitions,
    std::unordered_map<std::string, std::string>
        &implementation_method_symbols_by_owner_identity,
    std::string &error) {
  implementation_method_symbols_by_owner_identity.reserve(
      method_definitions.size());
  for (const Objc3IRMethodDefinition &method_def : method_definitions) {
    if (method_def.method_owner_identity.empty()) {
      continue;
    }
    const std::string implementation_symbol = "@" + method_def.symbol;
    const auto [binding_it, inserted] =
        implementation_method_symbols_by_owner_identity.emplace(
            method_def.method_owner_identity, implementation_symbol);
    if (!inserted && binding_it->second != implementation_symbol) {
      error = "duplicate executable method-body binding for owner identity '" +
              method_def.method_owner_identity + "'";
      return false;
    }
  }
  return true;
}

}  // namespace

bool EmitObjc3IRRuntimeMetadataSectionScaffold(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    std::ostringstream &out, std::string &error) {
  const Objc3IRFrontendMetadata &frontend_metadata = options.frontend_metadata;
  if (!Objc3IRRuntimeMetadataSectionScaffoldReady(frontend_metadata)) {
    return true;
  }

  Objc3RuntimeMetadataLayoutPolicy layout_policy;
  std::string layout_policy_error;
  if (!BuildObjc3IRRuntimeMetadataLayoutPolicy(
          frontend_metadata, layout_policy, layout_policy_error) ||
      !IsReadyObjc3RuntimeMetadataLayoutPolicy(layout_policy)) {
    return true;
  }

  const bool emit_class_metaclass_bundle_payloads =
      frontend_metadata.runtime_metadata_class_metaclass_emission_ready &&
      frontend_metadata.runtime_metadata_class_metaclass_emission_fail_closed &&
      !frontend_metadata
           .runtime_metadata_class_metaclass_emission_contract_id.empty() &&
      frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
              .size() == layout_policy.families[0].descriptor_count;
  const bool emit_protocol_category_bundle_payloads =
      frontend_metadata.runtime_metadata_protocol_category_emission_ready &&
      frontend_metadata.runtime_metadata_protocol_category_emission_fail_closed &&
      !frontend_metadata
           .runtime_metadata_protocol_category_emission_contract_id.empty() &&
      frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size() ==
          layout_policy.families[1].descriptor_count &&
      frontend_metadata.runtime_metadata_category_bundles_lexicographic.size() ==
          layout_policy.families[2].descriptor_count;
  const bool emit_member_table_payloads =
      frontend_metadata.runtime_metadata_member_table_emission_ready &&
      frontend_metadata.runtime_metadata_member_table_emission_fail_closed &&
      !frontend_metadata.runtime_metadata_member_table_emission_contract_id
           .empty() &&
      frontend_metadata.runtime_metadata_property_bundles_lexicographic.size() ==
          layout_policy.families[3].descriptor_count &&
      frontend_metadata.runtime_metadata_ivar_bundles_lexicographic.size() ==
          layout_policy.families[4].descriptor_count;

  EmitObjc3IRRuntimeMetadataScaffoldCommentSurfaces(
      options, layout_policy, emit_class_metaclass_bundle_payloads,
      emit_protocol_category_bundle_payloads, emit_member_table_payloads, out);
  out << "; runtime metadata section publication globals\n";

  std::vector<std::string> retained_globals;
  retained_globals.reserve(layout_policy.total_retained_global_count);

  std::unordered_map<std::string, std::string>
      protocol_descriptor_symbols_by_owner_identity;
  if (emit_protocol_category_bundle_payloads) {
    protocol_descriptor_symbols_by_owner_identity.reserve(
        frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size());
    for (std::size_t i = 0;
         i <
         frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size();
         ++i) {
      const auto &bundle =
          frontend_metadata.runtime_metadata_protocol_bundles_lexicographic[i];
      protocol_descriptor_symbols_by_owner_identity.emplace(
          bundle.owner_identity, BuildObjc3IRRuntimeMetadataDescriptorSymbol(
                                     layout_policy.descriptor_symbol_prefix,
                                     kObjc3RuntimeMetadataLayoutPolicyProtocolFamily,
                                     i));
    }
  }

  std::unordered_map<std::string, std::string> method_list_symbols_by_key;
  std::unordered_map<std::string, std::size_t>
      method_list_entry_counts_by_key;
  std::unordered_map<std::string, std::string>
      implementation_method_symbols_by_owner_identity;
  if (emit_member_table_payloads) {
    method_list_symbols_by_key.reserve(
        frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
            .size());
    method_list_entry_counts_by_key.reserve(
        frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
            .size());
    for (std::size_t i = 0;
         i <
         frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
             .size();
         ++i) {
      const auto &bundle =
          frontend_metadata.runtime_metadata_method_list_bundles_lexicographic[i];
      method_list_symbols_by_key.emplace(
          BuildMethodListKey(bundle.owner_family_kind,
                             bundle.declaration_owner_identity,
                             bundle.list_kind),
          BuildObjc3IRRuntimeMetadataAuxiliarySymbol(
              layout_policy.descriptor_symbol_prefix, bundle.owner_family_kind,
              bundle.list_kind + "_methods", i));
      method_list_entry_counts_by_key.emplace(
          BuildMethodListKey(bundle.owner_family_kind,
                             bundle.declaration_owner_identity,
                             bundle.list_kind),
          bundle.entries_lexicographic.size());
    }
    if (!BindImplementationMethodSymbols(
            options.method_definitions,
            implementation_method_symbols_by_owner_identity, error)) {
      return false;
    }
  }

  const std::string image_info_symbol = "@" + layout_policy.image_info_symbol;
  out << image_info_symbol << " = " << layout_policy.aggregate_linkage
      << " global { i32, i32 } zeroinitializer, section \""
      << layout_policy.emitted_image_info_section << "\", align 4\n";
  retained_globals.push_back(image_info_symbol);

  for (const auto &family : layout_policy.families) {
    if (emit_member_table_payloads &&
        (family.kind == kObjc3RuntimeMetadataLayoutPolicyClassFamily ||
         family.kind == kObjc3RuntimeMetadataLayoutPolicyProtocolFamily ||
         family.kind == kObjc3RuntimeMetadataLayoutPolicyCategoryFamily)) {
      if (!EmitObjc3IRRuntimeMethodListBundlesForFamily(
              Objc3IRRuntimeMemberMetadataEmissionOptions{
                  frontend_metadata, layout_policy, method_list_symbols_by_key,
                  implementation_method_symbols_by_owner_identity},
              family, out, retained_globals, error)) {
        if (error.empty()) {
          error = "runtime metadata method-body binding failed";
        }
        return false;
      }
    }
    if (emit_class_metaclass_bundle_payloads &&
        family.kind == kObjc3RuntimeMetadataLayoutPolicyClassFamily) {
      EmitObjc3IRRuntimeClassMetaclassBundleSection(
          Objc3IRRuntimeObjectMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              method_list_entry_counts_by_key,
              protocol_descriptor_symbols_by_owner_identity},
          family, out, retained_globals);
    } else if (emit_protocol_category_bundle_payloads &&
               family.kind == kObjc3RuntimeMetadataLayoutPolicyProtocolFamily) {
      EmitObjc3IRRuntimeProtocolBundleSection(
          Objc3IRRuntimeObjectMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              method_list_entry_counts_by_key,
              protocol_descriptor_symbols_by_owner_identity},
          family, out, retained_globals);
    } else if (emit_protocol_category_bundle_payloads &&
               family.kind == kObjc3RuntimeMetadataLayoutPolicyCategoryFamily) {
      EmitObjc3IRRuntimeCategoryBundleSection(
          Objc3IRRuntimeObjectMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              method_list_entry_counts_by_key,
              protocol_descriptor_symbols_by_owner_identity},
          family, out, retained_globals);
    } else if (emit_member_table_payloads &&
               family.kind == kObjc3RuntimeMetadataLayoutPolicyPropertyFamily) {
      EmitObjc3IRRuntimePropertyDescriptorSection(
          Objc3IRRuntimeMemberMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              implementation_method_symbols_by_owner_identity},
          family, out, retained_globals, error);
    } else if (emit_member_table_payloads &&
               family.kind == kObjc3RuntimeMetadataLayoutPolicyIvarFamily) {
      EmitObjc3IRRuntimeIvarDescriptorSection(
          Objc3IRRuntimeMemberMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              implementation_method_symbols_by_owner_identity},
          family, out, retained_globals);
    } else {
      EmitGenericDescriptorSection(layout_policy, family, out,
                                   retained_globals);
    }
  }

  return EmitObjc3IRRuntimeArtifacts(
      Objc3IRRuntimeArtifactEmissionOptions{
          options.module_name,
          frontend_metadata,
          options.runtime_metadata_symbols,
          layout_policy,
          options.selector_pool_globals,
          options.runtime_string_pool_globals,
          options.typed_keypath_artifacts,
          image_info_symbol,
          options.runtime_metadata_symbols.discovery_root_symbol,
          options.runtime_metadata_symbols.linker_anchor_symbol,
          options.emit_runtime_bootstrap_lowering,
          options.emit_runtime_bootstrap_registration_descriptor_image_root},
      out, retained_globals, error);
}
