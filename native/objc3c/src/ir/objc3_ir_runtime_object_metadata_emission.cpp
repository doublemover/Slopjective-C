#include "ir/objc3_ir_runtime_object_metadata_emission.h"

#include <cstddef>
#include <sstream>
#include <unordered_map>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

namespace {

void EmitRetained(std::vector<std::string> &retained_globals,
                  const std::string &symbol) {
  retained_globals.push_back(symbol);
}

std::string BuildRuntimeMetadataDescriptorSymbol(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family, std::size_t ordinal) {
  return BuildObjc3IRRuntimeMetadataDescriptorSymbol(
      layout_policy.descriptor_symbol_prefix, family.kind, ordinal);
}

std::string BuildRuntimeMetadataAuxiliarySymbol(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const std::string &suffix, std::size_t ordinal) {
  return BuildObjc3IRRuntimeMetadataAuxiliarySymbol(
      layout_policy.descriptor_symbol_prefix, family.kind, suffix, ordinal);
}

std::string BuildRuntimeMetadataAuxiliarySymbol(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const std::string &kind, const std::string &suffix, std::size_t ordinal) {
  return BuildObjc3IRRuntimeMetadataAuxiliarySymbol(
      layout_policy.descriptor_symbol_prefix, kind, suffix, ordinal);
}

std::string BuildMethodListKey(const std::string &owner_family_kind,
                               const std::string &owner_identity,
                               const std::string &list_kind) {
  return owner_family_kind + "|" + owner_identity + "|" + list_kind;
}

}  // namespace

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
        BuildRuntimeMetadataDescriptorSymbol(layout_policy, family, i));
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
        BuildRuntimeMetadataDescriptorSymbol(layout_policy, family, i);
    const std::string name_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family, "name", i);
    const std::string owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "owner_identity", i);
    const std::string class_object_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "class_object_identity", i);
    const std::string metaclass_object_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "metaclass_object_identity", i);
    const std::string instance_method_list_ref_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "instance_method_list_ref", i);
    const std::string metaclass_method_list_ref_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, "metaclass",
                                            "method_list_ref", i);
    const std::string adopted_protocol_refs_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "adopted_protocol_refs", i);
    std::string class_super_object_identity_symbol = "null";
    std::string metaclass_super_object_identity_symbol = "null";
    if (bundle.has_super) {
      class_super_object_identity_symbol =
          BuildRuntimeMetadataAuxiliarySymbol(
              layout_policy, family, "super_class_object_identity", i);
      metaclass_super_object_identity_symbol =
          BuildRuntimeMetadataAuxiliarySymbol(
              layout_policy, family, "super_metaclass_object_identity", i);
    }
    std::string instance_method_list_symbol = "null";
    const auto instance_method_list_it = method_list_symbols_by_key.find(
        BuildMethodListKey(family.kind, bundle.owner_identity, "instance"));
    if (instance_method_list_it != method_list_symbols_by_key.end()) {
      instance_method_list_symbol = instance_method_list_it->second;
    }
    std::size_t instance_method_list_entry_count = 0;
    const auto instance_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildMethodListKey(family.kind, bundle.owner_identity,
                               "instance"));
    if (instance_method_count_it != method_list_entry_counts_by_key.end()) {
      instance_method_list_entry_count = instance_method_count_it->second;
    }
    std::string metaclass_method_list_symbol = "null";
    const auto metaclass_method_list_it = method_list_symbols_by_key.find(
        BuildMethodListKey(family.kind, bundle.owner_identity, "class"));
    if (metaclass_method_list_it != method_list_symbols_by_key.end()) {
      metaclass_method_list_symbol = metaclass_method_list_it->second;
    }
    std::size_t metaclass_method_list_entry_count = 0;
    const auto metaclass_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildMethodListKey(family.kind, bundle.owner_identity, "class"));
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
    EmitRetained(retained_globals, descriptor_symbol);
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
  EmitRetained(retained_globals, aggregate_symbol);
}

void EmitObjc3IRRuntimeProtocolBundleSection(
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

  // protocol/category data emission anchor: protocol records now materialize
  // as real descriptor bundles with inherited protocol-ref lists inside the
  // protocol descriptor section.
  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(
      frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size());
  for (std::size_t i = 0;
       i < frontend_metadata.runtime_metadata_protocol_bundles_lexicographic
               .size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_protocol_bundles_lexicographic[i];
    const std::string descriptor_symbol =
        BuildRuntimeMetadataDescriptorSymbol(layout_policy, family, i);
    const std::string name_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family, "name", i);
    const std::string owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "owner_identity", i);
    const std::string inherited_refs_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "inherited_protocol_refs", i);
    const std::string instance_method_list_ref_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "instance_method_list_ref", i);
    const std::string class_method_list_ref_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "class_method_list_ref", i);
    std::string instance_method_list_symbol = "null";
    const auto instance_method_list_it = method_list_symbols_by_key.find(
        BuildMethodListKey(family.kind, bundle.owner_identity, "instance"));
    if (instance_method_list_it != method_list_symbols_by_key.end()) {
      instance_method_list_symbol = instance_method_list_it->second;
    }
    std::size_t instance_method_list_entry_count = 0;
    const auto instance_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildMethodListKey(family.kind, bundle.owner_identity,
                               "instance"));
    if (instance_method_count_it != method_list_entry_counts_by_key.end()) {
      instance_method_list_entry_count = instance_method_count_it->second;
    }
    std::string class_method_list_symbol = "null";
    const auto class_method_list_it = method_list_symbols_by_key.find(
        BuildMethodListKey(family.kind, bundle.owner_identity, "class"));
    if (class_method_list_it != method_list_symbols_by_key.end()) {
      class_method_list_symbol = class_method_list_it->second;
    }
    std::size_t class_method_list_entry_count = 0;
    const auto class_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildMethodListKey(family.kind, bundle.owner_identity, "class"));
    if (class_method_count_it != method_list_entry_counts_by_key.end()) {
      class_method_list_entry_count = class_method_count_it->second;
    }
    const std::size_t name_storage_len = bundle.protocol_name.size() + 1u;
    const std::size_t owner_identity_storage_len =
        bundle.owner_identity.size() + 1u;
    descriptor_symbols.push_back(descriptor_symbol);

    out << name_symbol << " = private constant [" << name_storage_len
        << " x i8] c\"" << EscapeCStringLiteral(bundle.protocol_name)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << owner_identity_symbol << " = private constant ["
        << owner_identity_storage_len << " x i8] c\""
        << EscapeCStringLiteral(bundle.owner_identity) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << inherited_refs_symbol << " = private global ";
    if (bundle.inherited_protocol_owner_identities_lexicographic.empty()) {
      out << "{ i64 } { i64 0 }";
    } else {
      out << "{ i64, ["
          << bundle.inherited_protocol_owner_identities_lexicographic.size()
          << " x ptr] } { i64 "
          << bundle.inherited_protocol_owner_identities_lexicographic.size()
          << ", ["
          << bundle.inherited_protocol_owner_identities_lexicographic.size()
          << " x ptr] [";
      for (std::size_t inherited_index = 0;
           inherited_index <
           bundle.inherited_protocol_owner_identities_lexicographic.size();
           ++inherited_index) {
        if (inherited_index != 0) {
          out << ", ";
        }
        std::string inherited_protocol_symbol = "null";
        const auto inherited_it =
            protocol_descriptor_symbols_by_owner_identity.find(
                bundle.inherited_protocol_owner_identities_lexicographic
                    [inherited_index]);
        if (inherited_it !=
            protocol_descriptor_symbols_by_owner_identity.end()) {
          inherited_protocol_symbol = inherited_it->second;
        }
        out << "ptr " << inherited_protocol_symbol;
      }
      out << "] }";
    }
    out << ", section \"" << family.emitted_section_name << "\", align 8\n";
    out << instance_method_list_ref_symbol
        << " = private global { i64, ptr, ptr } { i64 "
        << instance_method_list_entry_count << ", ptr "
        << owner_identity_symbol << ", ptr " << instance_method_list_symbol
        << " }, section \"" << family.emitted_section_name << "\", align 8\n";
    out << class_method_list_ref_symbol
        << " = private global { i64, ptr, ptr } { i64 "
        << class_method_list_entry_count << ", ptr " << owner_identity_symbol
        << ", ptr " << class_method_list_symbol << " }, section \""
        << family.emitted_section_name << "\", align 8\n";
    // executable realization-record expansion anchor: protocol records now
    // preserve split instance/class method counts alongside inherited protocol
    // edges so runtime conformance checks do not need to reconstruct that split
    // from sidecar summaries.
    out << descriptor_symbol
        << " = private global { ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i1 } { ptr "
        << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
        << inherited_refs_symbol << ", ptr " << instance_method_list_ref_symbol
        << ", ptr " << class_method_list_ref_symbol << ", i64 "
        << bundle.property_count << ", i64 " << bundle.method_count
        << ", i64 " << instance_method_list_entry_count << ", i64 "
        << class_method_list_entry_count << ", i1 "
        << (bundle.is_forward_declaration ? 1 : 0) << " }, section \""
        << family.emitted_section_name << "\", align 8\n";
    EmitRetained(retained_globals, descriptor_symbol);
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
  EmitRetained(retained_globals, aggregate_symbol);
}

void EmitObjc3IRRuntimeCategoryBundleSection(
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

  // protocol/category data emission anchor: category records now materialize as
  // interface/implementation descriptor bundles with attachment lists and
  // adopted protocol-ref lists inside the category descriptor section.
  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(
      frontend_metadata.runtime_metadata_category_bundles_lexicographic.size());
  for (std::size_t i = 0;
       i < frontend_metadata.runtime_metadata_category_bundles_lexicographic
               .size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_category_bundles_lexicographic[i];
    const std::string descriptor_symbol =
        BuildRuntimeMetadataDescriptorSymbol(layout_policy, family, i);
    const std::string class_name_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "class_name", i);
    const std::string category_name_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "category_name", i);
    const std::string owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "owner_identity", i);
    const std::string record_kind_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "record_kind", i);
    const std::string category_owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "category_owner_identity", i);
    const std::string class_owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "class_owner_identity", i);
    const std::string attachment_list_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "attachments", i);
    const std::string adopted_protocol_refs_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "adopted_protocol_refs", i);
    const std::string instance_method_list_ref_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "instance_method_list_ref", i);
    const std::string class_method_list_ref_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "class_method_list_ref", i);
    std::string instance_method_list_symbol = "null";
    const auto instance_method_list_it = method_list_symbols_by_key.find(
        BuildMethodListKey(family.kind, bundle.owner_identity, "instance"));
    if (instance_method_list_it != method_list_symbols_by_key.end()) {
      instance_method_list_symbol = instance_method_list_it->second;
    }
    std::size_t instance_method_list_entry_count = 0;
    const auto instance_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildMethodListKey(family.kind, bundle.owner_identity,
                               "instance"));
    if (instance_method_count_it != method_list_entry_counts_by_key.end()) {
      instance_method_list_entry_count = instance_method_count_it->second;
    }
    std::string class_method_list_symbol = "null";
    const auto class_method_list_it = method_list_symbols_by_key.find(
        BuildMethodListKey(family.kind, bundle.owner_identity, "class"));
    if (class_method_list_it != method_list_symbols_by_key.end()) {
      class_method_list_symbol = class_method_list_it->second;
    }
    std::size_t class_method_list_entry_count = 0;
    const auto class_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildMethodListKey(family.kind, bundle.owner_identity, "class"));
    if (class_method_count_it != method_list_entry_counts_by_key.end()) {
      class_method_list_entry_count = class_method_count_it->second;
    }
    descriptor_symbols.push_back(descriptor_symbol);

    out << class_name_symbol << " = private constant ["
        << (bundle.class_name.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.class_name) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << category_name_symbol << " = private constant ["
        << (bundle.category_name.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.category_name) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << owner_identity_symbol << " = private constant ["
        << (bundle.owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.owner_identity) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << record_kind_symbol << " = private constant ["
        << (bundle.record_kind.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.record_kind) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << category_owner_identity_symbol << " = private constant ["
        << (bundle.category_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.category_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << class_owner_identity_symbol << " = private constant ["
        << (bundle.class_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.class_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";

    out << attachment_list_symbol << " = private global ";
    const std::size_t attachment_count = 3u;
    out << "{ i64, [" << attachment_count << " x ptr] } { i64 "
        << attachment_count << ", [" << attachment_count << " x ptr] [";
    out << "ptr " << class_owner_identity_symbol << ", ptr "
        << category_owner_identity_symbol << ", ptr " << owner_identity_symbol;
    out << "] }, section \"" << family.emitted_section_name
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
    out << instance_method_list_ref_symbol
        << " = private global { i64, ptr, ptr } { i64 "
        << instance_method_list_entry_count << ", ptr " << owner_identity_symbol
        << ", ptr " << instance_method_list_symbol << " }, section \""
        << family.emitted_section_name << "\", align 8\n";
    out << class_method_list_ref_symbol
        << " = private global { i64, ptr, ptr } { i64 "
        << class_method_list_entry_count << ", ptr " << owner_identity_symbol
        << ", ptr " << class_method_list_symbol << " }, section \""
        << family.emitted_section_name << "\", align 8\n";

    // executable realization-record expansion anchor: category records now
    // preserve explicit class/category owner identities in-line while retaining
    // the earlier attachment and adopted-protocol aggregates for
    // backward-compatible proofing.
    out << descriptor_symbol
        << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 } "
           "{ ptr "
        << class_name_symbol << ", ptr " << category_name_symbol << ", ptr "
        << record_kind_symbol << ", ptr " << owner_identity_symbol << ", ptr "
        << class_owner_identity_symbol << ", ptr "
        << category_owner_identity_symbol << ", ptr " << attachment_list_symbol
        << ", ptr " << adopted_protocol_refs_symbol << ", ptr "
        << instance_method_list_ref_symbol << ", ptr "
        << class_method_list_ref_symbol << ", i64 " << bundle.property_count
        << ", i64 " << bundle.instance_method_count << ", i64 "
        << bundle.class_method_count << " }, section \""
        << family.emitted_section_name << "\", align 8\n";
    EmitRetained(retained_globals, descriptor_symbol);
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
  EmitRetained(retained_globals, aggregate_symbol);
}
