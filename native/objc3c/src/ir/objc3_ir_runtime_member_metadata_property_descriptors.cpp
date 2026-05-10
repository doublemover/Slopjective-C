#include "ir/objc3_ir_runtime_member_metadata_emission.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_runtime_member_metadata_symbols.h"
#include "ir/objc3_ir_symbol_model.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

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
        BuildObjc3IRRuntimeMemberMetadataDescriptorSymbol(layout_policy, family,
                                                          i);
    const std::string property_name_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(layout_policy, family,
                                                         "name", i);
    const std::string type_name_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(layout_policy, family,
                                                         "type", i);
    const std::string owner_identity_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "owner_identity", i);
    const std::string declaration_owner_identity_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "declaration_owner_identity", i);
    const std::string export_owner_identity_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "export_owner_identity", i);
    const std::string getter_symbol =
        bundle.has_getter
            ? BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "getter_selector", i)
            : std::string{"null"};
    const std::string setter_symbol =
        bundle.has_setter
            ? BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "setter_selector", i)
            : std::string{"null"};
    const std::string effective_getter_symbol =
        bundle.effective_getter_selector.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "effective_getter_selector", i);
    const std::string effective_setter_symbol =
        bundle.effective_setter_available
            ? BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "effective_setter_selector", i)
            : std::string{"null"};
    const std::string ivar_binding_symbol =
        bundle.ivar_binding_symbol.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "ivar_binding", i);
    const std::string synthesized_binding_symbol =
        bundle.executable_synthesized_binding_symbol.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "synthesized_binding", i);
    const std::string ivar_layout_symbol =
        bundle.executable_ivar_layout_symbol.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "ivar_layout", i);
    const std::string ivar_layout_replay_key_symbol =
        bundle.executable_ivar_layout_replay_key.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "ivar_layout_replay", i);
    const std::string property_attribute_profile_symbol =
        bundle.property_attribute_profile.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "property_attribute_profile", i);
    const std::string ownership_lifetime_profile_symbol =
        bundle.ownership_lifetime_profile.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "ownership_lifetime_profile", i);
    const std::string ownership_runtime_hook_profile_symbol =
        bundle.ownership_runtime_hook_profile.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
                  layout_policy, family, "ownership_runtime_hook_profile", i);
    const std::string accessor_ownership_profile_symbol =
        bundle.accessor_ownership_profile.empty()
            ? std::string{"null"}
            : BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
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
    RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals,
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
  RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals,
                                           aggregate_symbol);
  return true;
}
