#include "ir/objc3_ir_runtime_member_metadata_emission.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <sstream>
#include <utility>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "ir/objc3_ir_symbol_model.h"
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

std::string BuildMethodListKey(const std::string &owner_family_kind,
                               const std::string &owner_identity,
                               const std::string &list_kind) {
  return owner_family_kind + "|" + owner_identity + "|" + list_kind;
}

}  // namespace

bool EmitObjc3IRRuntimeMethodListBundlesForFamily(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    std::string &error) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;
  const auto &method_list_symbols_by_key = options.method_list_symbols_by_key;
  const auto &implementation_method_symbols_by_owner_identity =
      options.implementation_method_symbols_by_owner_identity;

  for (std::size_t bundle_index = 0;
       bundle_index <
       frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
           .size();
       ++bundle_index) {
    const auto &bundle =
        frontend_metadata
            .runtime_metadata_method_list_bundles_lexicographic[bundle_index];
    if (bundle.owner_family_kind != family.kind) {
      continue;
    }

    const auto method_list_it = method_list_symbols_by_key.find(
        BuildMethodListKey(bundle.owner_family_kind,
                           bundle.declaration_owner_identity,
                           bundle.list_kind));
    if (method_list_it == method_list_symbols_by_key.end()) {
      continue;
    }

    const std::string list_symbol = method_list_it->second;
    const std::string owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(
            layout_policy, family, bundle.list_kind + "_methods_owner_identity",
            bundle_index);
    const std::string export_owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(
            layout_policy, family,
            bundle.list_kind + "_methods_export_owner_identity", bundle_index);

    out << owner_identity_symbol << " = private constant ["
        << (bundle.declaration_owner_identity.size() + 1u)
        << " x i8] c\""
        << EscapeCStringLiteral(bundle.declaration_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << export_owner_identity_symbol << " = private constant ["
        << (bundle.export_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.export_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    std::vector<std::string> entry_initializers;
    entry_initializers.reserve(bundle.entries_lexicographic.size());
    for (std::size_t entry_index = 0;
         entry_index < bundle.entries_lexicographic.size(); ++entry_index) {
      const auto &entry = bundle.entries_lexicographic[entry_index];
      const std::string bundle_ordinal =
          FormatObjc3IRRuntimeMetadataDescriptorOrdinal(bundle_index);
      const std::string entry_ordinal =
          FormatObjc3IRRuntimeMetadataDescriptorOrdinal(entry_index);
      const std::string selector_symbol =
          "@" + layout_policy.descriptor_symbol_prefix + family.kind + "_" +
          bundle.list_kind + "_method_selector_" + bundle_ordinal + "_" +
          entry_ordinal;
      const std::string entry_owner_identity_symbol =
          "@" + layout_policy.descriptor_symbol_prefix + family.kind + "_" +
          bundle.list_kind + "_method_owner_identity_" + bundle_ordinal + "_" +
          entry_ordinal;
      const std::string return_type_symbol =
          "@" + layout_policy.descriptor_symbol_prefix + family.kind + "_" +
          bundle.list_kind + "_method_return_type_" + bundle_ordinal + "_" +
          entry_ordinal;
      std::string implementation_symbol = "null";
      if (entry.has_body &&
          (bundle.owner_kind == "class-implementation" ||
           bundle.owner_kind == "category-implementation")) {
        // executable method-body binding anchor: implementation-owned method
        // entries must resolve to one emitted LLVM body symbol.
        const auto implementation_it =
            implementation_method_symbols_by_owner_identity.find(
                entry.owner_identity);
        if (implementation_it ==
            implementation_method_symbols_by_owner_identity.end()) {
          error =
              "missing executable method-body binding for owner identity '" +
              entry.owner_identity + "' in emitted " + bundle.owner_kind +
              " " + bundle.list_kind + " method list";
          return false;
        }
        implementation_symbol = implementation_it->second;
      }

      out << selector_symbol << " = private constant ["
          << (entry.selector.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(entry.selector) << "\\00\", section \""
          << family.emitted_section_name << "\", align 1\n";
      out << entry_owner_identity_symbol << " = private constant ["
          << (entry.owner_identity.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(entry.owner_identity)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
      out << return_type_symbol << " = private constant ["
          << (entry.return_type_name.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(entry.return_type_name)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
      std::ostringstream entry_initializer;
      // dispatch-control lowering anchor: method entries preserve
      // effective-direct and objc_final intent bits alongside implementation
      // bindings.
      entry_initializer << "{ ptr, ptr, ptr, i64, ptr, i64, i1, i1 } { ptr "
                        << selector_symbol << ", ptr "
                        << entry_owner_identity_symbol << ", ptr "
                        << return_type_symbol << ", i64 "
                        << entry.parameter_count << ", ptr "
                        << implementation_symbol << ", i64 "
                        << (entry.has_body ? 1 : 0) << ", i1 "
                        << (entry.effective_direct_dispatch ? 1 : 0)
                        << ", i1 "
                        << (entry.objc_final_declared ? 1 : 0) << " }";
      entry_initializers.push_back(entry_initializer.str());
    }
    out << list_symbol << " = private global ";
    if (entry_initializers.empty()) {
      out << "{ i64, ptr, ptr } { i64 0, ptr " << owner_identity_symbol
          << ", ptr " << export_owner_identity_symbol << " }";
    } else {
      out << "{ i64, ptr, ptr, [" << entry_initializers.size()
          << " x { ptr, ptr, ptr, i64, ptr, i64, i1, i1 }] } { i64 "
          << entry_initializers.size() << ", ptr " << owner_identity_symbol
          << ", ptr " << export_owner_identity_symbol << ", ["
          << entry_initializers.size()
          << " x { ptr, ptr, ptr, i64, ptr, i64, i1, i1 }] [";
      for (std::size_t entry_index = 0;
           entry_index < entry_initializers.size(); ++entry_index) {
        if (entry_index != 0) {
          out << ", ";
        }
        out << entry_initializers[entry_index];
      }
      out << "] }";
    }
    out << ", section \"" << family.emitted_section_name
        << "\", align 8\n";
    EmitRetained(retained_globals, list_symbol);
  }
  return true;
}

bool EmitObjc3IRRuntimePropertyDescriptorSection(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    std::string &error) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;
  const auto &implementation_method_symbols_by_owner_identity =
      options.implementation_method_symbols_by_owner_identity;

  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(
      frontend_metadata.runtime_metadata_property_bundles_lexicographic.size());
  for (std::size_t i = 0;
       i < frontend_metadata.runtime_metadata_property_bundles_lexicographic
               .size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_property_bundles_lexicographic[i];
    const std::string descriptor_symbol =
        BuildRuntimeMetadataDescriptorSymbol(layout_policy, family, i);
    const std::string property_name_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family, "name", i);
    const std::string type_name_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family, "type", i);
    const std::string owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "owner_identity", i);
    const std::string declaration_owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(
            layout_policy, family, "declaration_owner_identity", i);
    const std::string export_owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "export_owner_identity", i);
    const std::string getter_symbol =
        bundle.has_getter
            ? BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                                  "getter_selector", i)
            : std::string{"null"};
    const std::string setter_symbol =
        bundle.has_setter
            ? BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                                  "setter_selector", i)
            : std::string{"null"};
    const std::string effective_getter_symbol =
        bundle.effective_getter_selector.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(
                  layout_policy, family, "effective_getter_selector", i);
    const std::string effective_setter_symbol =
        bundle.effective_setter_available
            ? BuildRuntimeMetadataAuxiliarySymbol(
                  layout_policy, family, "effective_setter_selector", i)
            : std::string{"null"};
    const std::string ivar_binding_symbol =
        bundle.ivar_binding_symbol.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                                  "ivar_binding", i);
    const std::string synthesized_binding_symbol =
        bundle.executable_synthesized_binding_symbol.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                                  "synthesized_binding", i);
    const std::string ivar_layout_symbol =
        bundle.executable_ivar_layout_symbol.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                                  "ivar_layout", i);
    const std::string ivar_layout_replay_key_symbol =
        bundle.executable_ivar_layout_replay_key.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                                  "ivar_layout_replay", i);
    const std::string property_attribute_profile_symbol =
        bundle.property_attribute_profile.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(
                  layout_policy, family, "property_attribute_profile", i);
    const std::string ownership_lifetime_profile_symbol =
        bundle.ownership_lifetime_profile.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(
                  layout_policy, family, "ownership_lifetime_profile", i);
    const std::string ownership_runtime_hook_profile_symbol =
        bundle.ownership_runtime_hook_profile.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(
                  layout_policy, family, "ownership_runtime_hook_profile", i);
    const std::string accessor_ownership_profile_symbol =
        bundle.accessor_ownership_profile.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(
                  layout_policy, family, "accessor_ownership_profile", i);
    std::string getter_implementation_symbol = "null";
    std::string setter_implementation_symbol = "null";
    descriptor_symbols.push_back(descriptor_symbol);

    out << property_name_symbol << " = private constant ["
        << (bundle.property_name.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.property_name) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << type_name_symbol << " = private constant ["
        << (bundle.type_name.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.type_name) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << owner_identity_symbol << " = private constant ["
        << (bundle.owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << declaration_owner_identity_symbol << " = private constant ["
        << (bundle.declaration_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.declaration_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << export_owner_identity_symbol << " = private constant ["
        << (bundle.export_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.export_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    if (bundle.has_getter) {
      out << getter_symbol << " = private constant ["
          << (bundle.getter_selector.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(bundle.getter_selector)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (bundle.has_setter) {
      out << setter_symbol << " = private constant ["
          << (bundle.setter_selector.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(bundle.setter_selector)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.effective_getter_selector.empty()) {
      out << effective_getter_symbol << " = private constant ["
          << (bundle.effective_getter_selector.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(bundle.effective_getter_selector)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (bundle.effective_setter_available) {
      out << effective_setter_symbol << " = private constant ["
          << (bundle.effective_setter_selector.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(bundle.effective_setter_selector)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.ivar_binding_symbol.empty()) {
      out << ivar_binding_symbol << " = private constant ["
          << (bundle.ivar_binding_symbol.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(bundle.ivar_binding_symbol)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.executable_synthesized_binding_symbol.empty()) {
      out << synthesized_binding_symbol << " = private constant ["
          << (bundle.executable_synthesized_binding_symbol.size() + 1u)
          << " x i8] c\""
          << EscapeCStringLiteral(bundle.executable_synthesized_binding_symbol)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.executable_ivar_layout_symbol.empty()) {
      out << ivar_layout_symbol << " = private constant ["
          << (bundle.executable_ivar_layout_symbol.size() + 1u)
          << " x i8] c\""
          << EscapeCStringLiteral(bundle.executable_ivar_layout_symbol)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.executable_ivar_layout_replay_key.empty()) {
      out << ivar_layout_replay_key_symbol << " = private constant ["
          << (bundle.executable_ivar_layout_replay_key.size() + 1u)
          << " x i8] c\""
          << EscapeCStringLiteral(bundle.executable_ivar_layout_replay_key)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.property_attribute_profile.empty()) {
      out << property_attribute_profile_symbol << " = private constant ["
          << (bundle.property_attribute_profile.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(bundle.property_attribute_profile)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.ownership_lifetime_profile.empty()) {
      out << ownership_lifetime_profile_symbol << " = private constant ["
          << (bundle.ownership_lifetime_profile.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(bundle.ownership_lifetime_profile)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.ownership_runtime_hook_profile.empty()) {
      out << ownership_runtime_hook_profile_symbol << " = private constant ["
          << (bundle.ownership_runtime_hook_profile.size() + 1u)
          << " x i8] c\""
          << EscapeCStringLiteral(bundle.ownership_runtime_hook_profile)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.accessor_ownership_profile.empty()) {
      out << accessor_ownership_profile_symbol << " = private constant ["
          << (bundle.accessor_ownership_profile.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(bundle.accessor_ownership_profile)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (Objc3IRRuntimeMetadataPropertyBundleIsImplementationOwned(bundle)) {
      const auto getter_implementation_it =
          implementation_method_symbols_by_owner_identity.find(
              BuildSynthesizedInstanceMethodOwnerIdentity(
                  bundle.declaration_owner_identity,
                  bundle.effective_getter_selector));
      if (getter_implementation_it ==
          implementation_method_symbols_by_owner_identity.end()) {
        error = "missing synthesized accessor getter binding for property '" +
                bundle.property_name + "' in owner '" +
                bundle.declaration_owner_identity + "'";
        return false;
      }
      getter_implementation_symbol = getter_implementation_it->second;
      if (bundle.effective_setter_available) {
        const auto setter_implementation_it =
            implementation_method_symbols_by_owner_identity.find(
                BuildSynthesizedInstanceMethodOwnerIdentity(
                    bundle.declaration_owner_identity,
                    bundle.effective_setter_selector));
        if (setter_implementation_it ==
            implementation_method_symbols_by_owner_identity.end()) {
          error = "missing synthesized accessor setter binding for property '" +
                  bundle.property_name + "' in owner '" +
                  bundle.declaration_owner_identity + "'";
          return false;
        }
        setter_implementation_symbol = setter_implementation_it->second;
      }
    }

    out << descriptor_symbol
        << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1, i1, i1, i1 } "
           "{ ptr "
        << property_name_symbol << ", ptr " << type_name_symbol << ", ptr "
        << owner_identity_symbol << ", ptr "
        << declaration_owner_identity_symbol << ", ptr "
        << export_owner_identity_symbol << ", ptr " << getter_symbol
        << ", ptr " << setter_symbol << ", ptr " << effective_getter_symbol
        << ", ptr " << effective_setter_symbol << ", ptr "
        << ivar_binding_symbol << ", ptr " << synthesized_binding_symbol
        << ", ptr " << ivar_layout_symbol << ", ptr "
        << property_attribute_profile_symbol << ", ptr "
        << ownership_lifetime_profile_symbol << ", ptr "
        << ownership_runtime_hook_profile_symbol << ", ptr "
        << accessor_ownership_profile_symbol << ", ptr "
        << getter_implementation_symbol << ", ptr "
        << setter_implementation_symbol << ", ptr "
        << ivar_layout_replay_key_symbol << ", i64 "
        << bundle.executable_ivar_layout_slot_index << ", i64 "
        << bundle.executable_ivar_layout_size_bytes << ", i64 "
        << bundle.executable_ivar_layout_alignment_bytes << ", i64 "
        << bundle.executable_ivar_layout_offset_bytes << ", i64 "
        << bundle.executable_ivar_layout_padding_bytes << ", i64 "
        << bundle.executable_ivar_layout_inherited_slot_count << ", i64 "
        << bundle.executable_ivar_layout_inherited_size_bytes << ", i64 "
        << bundle.executable_ivar_layout_owner_size_bytes << ", i64 "
        << bundle.executable_ivar_init_order_index << ", i64 "
        << bundle.executable_ivar_destroy_order_index << ", i1 "
        << (bundle.executable_ivar_layout_valid ? 1 : 0) << ", i1 "
        << (bundle.has_getter ? 1 : 0) << ", i1 "
        << (bundle.has_setter ? 1 : 0) << ", i1 "
        << (bundle.effective_setter_available ? 1 : 0) << " }, section \""
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
  return true;
}

void EmitObjc3IRRuntimeIvarDescriptorSection(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;
  std::map<std::string, std::size_t> instance_size_by_owner_identity;

  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(
      frontend_metadata.runtime_metadata_ivar_bundles_lexicographic.size());
  std::map<std::string, std::vector<std::pair<std::size_t, std::string>>>
      descriptor_symbols_by_declaration_owner_identity;
  for (std::size_t i = 0;
       i <
       frontend_metadata.runtime_metadata_ivar_bundles_lexicographic.size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_ivar_bundles_lexicographic[i];
    const std::string descriptor_symbol =
        BuildRuntimeMetadataDescriptorSymbol(layout_policy, family, i);
    const std::string owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "owner_identity", i);
    const std::string declaration_owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(
            layout_policy, family, "declaration_owner_identity", i);
    const std::string export_owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "export_owner_identity", i);
    const std::string property_owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "property_owner_identity", i);
    const std::string property_name_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "property_name", i);
    const std::string ivar_binding_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family, "binding",
                                            i);
    const std::string layout_symbol =
        bundle.executable_ivar_layout_symbol.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                                  "layout_symbol", i);
    const std::string offset_global_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family, "offset",
                                            i);
    const std::string layout_record_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "layout_record", i);
    const std::string layout_replay_key_symbol =
        bundle.executable_ivar_layout_replay_key.empty()
            ? std::string{"null"}
            : BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                                  "layout_replay", i);
    descriptor_symbols.push_back(descriptor_symbol);
    descriptor_symbols_by_declaration_owner_identity
        [bundle.declaration_owner_identity]
            .push_back(std::make_pair(
                bundle.executable_ivar_layout_slot_index, descriptor_symbol));
    instance_size_by_owner_identity[bundle.declaration_owner_identity] =
        std::max(
            instance_size_by_owner_identity[bundle.declaration_owner_identity],
            bundle.executable_ivar_layout_owner_size_bytes);

    out << owner_identity_symbol << " = private constant ["
        << (bundle.owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << declaration_owner_identity_symbol << " = private constant ["
        << (bundle.declaration_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.declaration_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << export_owner_identity_symbol << " = private constant ["
        << (bundle.export_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.export_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << property_owner_identity_symbol << " = private constant ["
        << (bundle.property_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.property_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << property_name_symbol << " = private constant ["
        << (bundle.property_name.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.property_name)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << ivar_binding_symbol << " = private constant ["
        << (bundle.ivar_binding_symbol.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.ivar_binding_symbol)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    if (!bundle.executable_ivar_layout_symbol.empty()) {
      out << layout_symbol << " = private constant ["
          << (bundle.executable_ivar_layout_symbol.size() + 1u)
          << " x i8] c\""
          << EscapeCStringLiteral(bundle.executable_ivar_layout_symbol)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    if (!bundle.executable_ivar_layout_replay_key.empty()) {
      out << layout_replay_key_symbol << " = private constant ["
          << (bundle.executable_ivar_layout_replay_key.size() + 1u)
          << " x i8] c\""
          << EscapeCStringLiteral(bundle.executable_ivar_layout_replay_key)
          << "\\00\", section \"" << family.emitted_section_name
          << "\", align 1\n";
    }
    out << offset_global_symbol << " = private global i64 "
        << bundle.executable_ivar_layout_offset_bytes << ", section \""
        << family.emitted_section_name << "\", align 8\n";
    EmitRetained(retained_globals, offset_global_symbol);
    out << layout_record_symbol
        << " = private global { ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1 } { ptr "
        << layout_symbol << ", ptr " << layout_replay_key_symbol << ", i64 "
        << bundle.executable_ivar_layout_slot_index << ", i64 "
        << bundle.executable_ivar_layout_offset_bytes << ", i64 "
        << bundle.executable_ivar_layout_size_bytes << ", i64 "
        << bundle.executable_ivar_layout_alignment_bytes << ", i64 "
        << bundle.executable_ivar_layout_padding_bytes << ", i64 "
        << bundle.executable_ivar_layout_inherited_slot_count << ", i64 "
        << bundle.executable_ivar_layout_inherited_size_bytes << ", i64 "
        << bundle.executable_ivar_layout_owner_size_bytes << ", i64 "
        << bundle.executable_ivar_init_order_index << ", i64 "
        << bundle.executable_ivar_destroy_order_index << ", i1 "
        << (bundle.executable_ivar_layout_valid ? 1 : 0)
        << " }, section \"" << family.emitted_section_name
        << "\", align 8\n";
    EmitRetained(retained_globals, layout_record_symbol);

    out << descriptor_symbol
        << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1 } { ptr "
        << owner_identity_symbol << ", ptr " << declaration_owner_identity_symbol
        << ", ptr " << export_owner_identity_symbol << ", ptr "
        << property_owner_identity_symbol << ", ptr " << property_name_symbol
        << ", ptr " << ivar_binding_symbol << ", ptr "
        << layout_record_symbol << ", ptr " << offset_global_symbol
        << ", ptr " << layout_replay_key_symbol << ", i64 "
        << bundle.executable_ivar_layout_slot_index << ", i64 "
        << bundle.executable_ivar_layout_offset_bytes << ", i64 "
        << bundle.executable_ivar_layout_size_bytes << ", i64 "
        << bundle.executable_ivar_layout_alignment_bytes << ", i64 "
        << bundle.executable_ivar_layout_padding_bytes << ", i64 "
        << bundle.executable_ivar_layout_inherited_slot_count << ", i64 "
        << bundle.executable_ivar_layout_inherited_size_bytes << ", i64 "
        << bundle.executable_ivar_layout_owner_size_bytes << ", i64 "
        << bundle.executable_ivar_init_order_index << ", i64 "
        << bundle.executable_ivar_destroy_order_index << ", i1 "
        << (bundle.executable_ivar_layout_valid ? 1 : 0)
        << " }, section \"" << family.emitted_section_name
        << "\", align 8\n";
    EmitRetained(retained_globals, descriptor_symbol);
  }

  std::size_t layout_table_ordinal = 0u;
  for (auto &[owner_identity, owner_descriptor_symbols] :
       descriptor_symbols_by_declaration_owner_identity) {
    std::stable_sort(owner_descriptor_symbols.begin(),
                     owner_descriptor_symbols.end(),
                     [](const auto &lhs, const auto &rhs) {
                       return lhs.first < rhs.first ||
                              (lhs.first == rhs.first &&
                               lhs.second < rhs.second);
                     });
    const std::string owner_identity_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "layout_owner",
                                            layout_table_ordinal);
    const std::string layout_table_symbol =
        BuildRuntimeMetadataAuxiliarySymbol(layout_policy, family,
                                            "layout_table",
                                            layout_table_ordinal);
    out << owner_identity_symbol << " = private constant ["
        << (owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(owner_identity) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << layout_table_symbol << " = private global ";
    if (owner_descriptor_symbols.empty()) {
      out << "{ ptr, i64, i64 } { ptr " << owner_identity_symbol
          << ", i64 0, i64 0 }";
    } else {
      out << "{ ptr, i64, [" << owner_descriptor_symbols.size()
          << " x ptr], i64 } { ptr " << owner_identity_symbol << ", i64 "
          << owner_descriptor_symbols.size() << ", ["
          << owner_descriptor_symbols.size() << " x ptr] [";
      for (std::size_t symbol_index = 0u;
           symbol_index < owner_descriptor_symbols.size(); ++symbol_index) {
        if (symbol_index != 0u) {
          out << ", ";
        }
        out << "ptr " << owner_descriptor_symbols[symbol_index].second;
      }
      out << "], i64 " << instance_size_by_owner_identity[owner_identity]
          << " }";
    }
    out << ", section \"" << family.emitted_section_name << "\", align 8\n";
    EmitRetained(retained_globals, layout_table_symbol);
    ++layout_table_ordinal;
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
