#include "ir/objc3_ir_runtime_object_metadata_emission.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_object_metadata_symbols.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

void EmitObjc3IRRuntimeClassMetaclassBundleSection(
    const Objc3IRRuntimeObjectMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;
  const auto &method_list_symbols_by_key = options.method_list_symbols_by_key;
  const auto &method_list_entry_counts_by_key =
      options.method_list_entry_counts_by_key;
  const auto &protocol_descriptor_symbols_by_owner_identity =
      options.protocol_descriptor_symbols_by_owner_identity;

  std::unordered_map<std::string, std::string>
      descriptor_symbols_by_owner_identity;
  descriptor_symbols_by_owner_identity.reserve(
      frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
          .size());
  for (std::size_t i = 0;
       i < frontend_metadata
               .runtime_metadata_class_metaclass_bundles_lexicographic.size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
            [i];
    descriptor_symbols_by_owner_identity.emplace(
        bundle.owner_identity,
        BuildObjc3IRRuntimeObjectMetadataDescriptorSymbol(layout_policy, family,
                                                          i));
  }

  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(
      frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
          .size());
  for (std::size_t i = 0;
       i < frontend_metadata
               .runtime_metadata_class_metaclass_bundles_lexicographic.size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
            [i];
    const std::string descriptor_symbol =
        BuildObjc3IRRuntimeObjectMetadataDescriptorSymbol(layout_policy, family,
                                                          i);
    const std::string name_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(layout_policy, family,
                                                         "name", i);
    const std::string owner_identity_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "owner_identity", i);
    const std::string class_object_identity_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "class_object_identity", i);
    const std::string metaclass_object_identity_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "metaclass_object_identity", i);
    const std::string instance_method_list_ref_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "instance_method_list_ref", i);
    const std::string metaclass_method_list_ref_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, "metaclass", "method_list_ref", i);
    const std::string adopted_protocol_refs_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "adopted_protocol_refs", i);
    std::string class_super_object_identity_symbol = "null";
    std::string metaclass_super_object_identity_symbol = "null";
    if (bundle.has_super) {
      class_super_object_identity_symbol =
          BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
              layout_policy, family, "super_class_object_identity", i);
      metaclass_super_object_identity_symbol =
          BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
              layout_policy, family, "super_metaclass_object_identity", i);
    }
    std::string instance_method_list_symbol = "null";
    const auto instance_method_list_it = method_list_symbols_by_key.find(
        BuildObjc3IRRuntimeObjectMetadataMethodListKey(
            family.kind, bundle.owner_identity, "instance"));
    if (instance_method_list_it != method_list_symbols_by_key.end()) {
      instance_method_list_symbol = instance_method_list_it->second;
    }
    std::size_t instance_method_list_entry_count = 0;
    const auto instance_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildObjc3IRRuntimeObjectMetadataMethodListKey(
                family.kind, bundle.owner_identity, "instance"));
    if (instance_method_count_it != method_list_entry_counts_by_key.end()) {
      instance_method_list_entry_count = instance_method_count_it->second;
    }
    std::string metaclass_method_list_symbol = "null";
    const auto metaclass_method_list_it = method_list_symbols_by_key.find(
        BuildObjc3IRRuntimeObjectMetadataMethodListKey(
            family.kind, bundle.owner_identity, "class"));
    if (metaclass_method_list_it != method_list_symbols_by_key.end()) {
      metaclass_method_list_symbol = metaclass_method_list_it->second;
    }
    std::size_t metaclass_method_list_entry_count = 0;
    const auto metaclass_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildObjc3IRRuntimeObjectMetadataMethodListKey(
                family.kind, bundle.owner_identity, "class"));
    if (metaclass_method_count_it !=
        method_list_entry_counts_by_key.end()) {
      metaclass_method_list_entry_count = metaclass_method_count_it->second;
    }
    std::string super_bundle_symbol = "null";
    if (bundle.has_super) {
      const auto super_it = descriptor_symbols_by_owner_identity.find(
          bundle.super_bundle_owner_identity);
      if (super_it != descriptor_symbols_by_owner_identity.end()) {
        super_bundle_symbol = super_it->second;
      }
    }
    const std::size_t storage_len = bundle.class_name.size() + 1u;
    const std::size_t owner_identity_storage_len =
        bundle.owner_identity.size() + 1u;
    const std::size_t class_object_identity_storage_len =
        bundle.class_owner_identity.size() + 1u;
    const std::size_t metaclass_object_identity_storage_len =
        bundle.metaclass_owner_identity.size() + 1u;
    descriptor_symbols.push_back(descriptor_symbol);

    out << name_symbol << " = private constant [" << storage_len
        << " x i8] c\"" << EscapeCStringLiteral(bundle.class_name)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << owner_identity_symbol << " = private constant ["
        << owner_identity_storage_len << " x i8] c\""
        << EscapeCStringLiteral(bundle.owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << class_object_identity_symbol << " = private constant ["
        << class_object_identity_storage_len << " x i8] c\""
        << EscapeCStringLiteral(bundle.class_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << metaclass_object_identity_symbol << " = private constant ["
        << metaclass_object_identity_storage_len << " x i8] c\""
        << EscapeCStringLiteral(bundle.metaclass_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    if (bundle.has_super) {
      out << class_super_object_identity_symbol << " = private constant ["
          << (bundle.super_class_owner_identity.size() + 1u)
          << " x i8] c\""
          << EscapeCStringLiteral(bundle.super_class_owner_identity)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
      out << metaclass_super_object_identity_symbol << " = private constant ["
          << (bundle.super_metaclass_owner_identity.size() + 1u)
          << " x i8] c\""
          << EscapeCStringLiteral(bundle.super_metaclass_owner_identity)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    out << instance_method_list_ref_symbol
        << " = private global { i64, ptr, ptr } { i64 "
        << instance_method_list_entry_count << ", ptr "
        << owner_identity_symbol << ", ptr " << instance_method_list_symbol
        << " }, section \"" << family.emitted_section_name
        << "\", align 8\n";
    out << metaclass_method_list_ref_symbol
        << " = private global { i64, ptr, ptr } { i64 "
        << metaclass_method_list_entry_count << ", ptr "
        << owner_identity_symbol << ", ptr " << metaclass_method_list_symbol
        << " }, section \"" << family.emitted_section_name
        << "\", align 8\n";
    out << adopted_protocol_refs_symbol << " = private global ";
    if (bundle.adopted_protocol_owner_identities_lexicographic.empty()) {
      out << "{ i64 } { i64 0 }";
    } else {
      out << "{ i64, ["
          << bundle.adopted_protocol_owner_identities_lexicographic.size()
          << " x ptr] } { i64 "
          << bundle.adopted_protocol_owner_identities_lexicographic.size()
          << ", ["
          << bundle.adopted_protocol_owner_identities_lexicographic.size()
          << " x ptr] [";
      for (std::size_t adopted_index = 0;
           adopted_index <
           bundle.adopted_protocol_owner_identities_lexicographic.size();
           ++adopted_index) {
        if (adopted_index != 0) {
          out << ", ";
        }
        std::string adopted_protocol_symbol = "null";
        const auto adopted_it =
            protocol_descriptor_symbols_by_owner_identity.find(
                bundle.adopted_protocol_owner_identities_lexicographic
                    [adopted_index]);
        if (adopted_it !=
            protocol_descriptor_symbols_by_owner_identity.end()) {
          adopted_protocol_symbol = adopted_it->second;
        }
        out << "ptr " << adopted_protocol_symbol;
      }
      out << "] }";
    }
    out << ", section \"" << family.emitted_section_name << "\", align 8\n";
    // executable realization-record expansion anchor: class/metaclass records
    // preserve bundle-owner, object-owner, super-object identities, method-list
    // refs, and container intent bits in-line.
    out << descriptor_symbol
        << " = private global { { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } } "
           "{ { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } { ptr "
        << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
        << class_object_identity_symbol << ", ptr "
        << class_super_object_identity_symbol << ", ptr "
        << super_bundle_symbol << ", ptr " << instance_method_list_ref_symbol
        << ", ptr " << adopted_protocol_refs_symbol << ", i1 "
        << (bundle.objc_final_declared ? 1 : 0) << ", i1 "
        << (bundle.objc_sealed_declared ? 1 : 0)
        << " }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } { ptr "
        << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
        << metaclass_object_identity_symbol << ", ptr "
        << metaclass_super_object_identity_symbol << ", ptr "
        << super_bundle_symbol << ", ptr " << metaclass_method_list_ref_symbol
        << ", ptr " << adopted_protocol_refs_symbol << ", i1 "
        << (bundle.objc_final_declared ? 1 : 0) << ", i1 "
        << (bundle.objc_sealed_declared ? 1 : 0) << " } }, section \""
        << family.emitted_section_name << "\", align 8\n";
    RetainObjc3IRRuntimeObjectMetadataGlobal(retained_globals,
                                             descriptor_symbol);
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
  RetainObjc3IRRuntimeObjectMetadataGlobal(retained_globals,
                                           aggregate_symbol);
}
