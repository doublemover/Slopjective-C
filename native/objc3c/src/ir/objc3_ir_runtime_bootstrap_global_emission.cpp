#include "ir/objc3_ir_runtime_bootstrap_global_emission.h"

#include "ast/objc3_ast_contracts.h"
#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

#include <cstdint>
#include <limits>
#include <sstream>

void EmitObjc3IRRuntimeBootstrapGlobals(
    const Objc3IRFrontendMetadata &frontend_metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    const Objc3IRRuntimeBootstrapGlobalEmissionOptions &options,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  const std::string module_name_symbol =
      "@" + runtime_metadata_symbols.module_name_global_symbol;
  const std::string translation_unit_identity_symbol =
      "@" + runtime_metadata_symbols.translation_unit_identity_global_symbol;
  const std::string image_descriptor_symbol =
      "@" + runtime_metadata_symbols.image_descriptor_symbol;
  const std::string registration_descriptor_name_symbol =
      options.emit_registration_descriptor_image_root
          ? "@" + runtime_metadata_symbols.registration_descriptor_name_global_symbol
          : std::string();
  const std::string image_root_name_symbol =
      options.emit_registration_descriptor_image_root
          ? "@" + runtime_metadata_symbols.image_root_name_global_symbol
          : std::string();
  const std::string registration_descriptor_symbol =
      options.emit_registration_descriptor_image_root
          ? "@" + runtime_metadata_symbols.registration_descriptor_symbol
          : std::string();
  const std::string image_root_symbol =
      options.emit_registration_descriptor_image_root
          ? "@" + runtime_metadata_symbols.image_root_symbol
          : std::string();
  const std::string registration_table_symbol =
      "@" + runtime_metadata_symbols.registration_table_symbol;
  const std::string image_local_init_state_symbol =
      "@" + runtime_metadata_symbols.image_local_init_state_symbol;
  const std::string constructor_root_symbol =
      "@" + frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol;
  const std::string class_section_root_symbol = "@__objc3_sec_class_descriptors";
  const std::string protocol_section_root_symbol =
      "@__objc3_sec_protocol_descriptors";
  const std::string category_section_root_symbol =
      "@__objc3_sec_category_descriptors";
  const std::string property_section_root_symbol =
      "@__objc3_sec_property_descriptors";
  const std::string ivar_section_root_symbol = "@__objc3_sec_ivar_descriptors";
  const std::string selector_pool_symbol =
      options.emit_selector_string_pools ? "@__objc3_sec_selector_pool" : "null";
  const std::string string_pool_symbol =
      options.emit_selector_string_pools ? "@__objc3_sec_string_pool" : "null";
  const std::string keypath_descriptor_root_symbol =
      options.emit_typed_keypath_artifacts ? "@__objc3_sec_keypath_descriptors"
                                           : "null";
  const std::string module_name =
      options.module_name.empty() ? "objc3_module" : options.module_name;
  const std::string &translation_unit_identity_key =
      frontend_metadata
          .runtime_metadata_archive_static_link_translation_unit_identity_key;
  const std::string &registration_descriptor_identifier =
      frontend_metadata.runtime_bootstrap_registration_descriptor_identifier;
  const std::string &image_root_identifier =
      frontend_metadata.runtime_bootstrap_image_root_identifier;

  out << module_name_symbol << " = private unnamed_addr constant ["
      << (module_name.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(module_name) << "\\00\", align 1\n";
  out << translation_unit_identity_symbol
      << " = private unnamed_addr constant ["
      << (translation_unit_identity_key.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(translation_unit_identity_key)
      << "\\00\", align 1\n";
  if (options.emit_registration_descriptor_image_root) {
    out << registration_descriptor_name_symbol
        << " = private unnamed_addr constant ["
        << (registration_descriptor_identifier.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(registration_descriptor_identifier)
        << "\\00\", align 1\n";
    out << image_root_name_symbol << " = private unnamed_addr constant ["
        << (image_root_identifier.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(image_root_identifier) << "\\00\", align 1\n";
  }

  const std::uint64_t registration_order_ordinal =
      frontend_metadata.runtime_bootstrap_registration_order_ordinal == 0
          ? kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal
          : frontend_metadata.runtime_bootstrap_registration_order_ordinal;
  out << image_descriptor_symbol << " = internal constant "
      << Objc3IRRuntimeBootstrapImageDescriptorType()
      << " { ptr getelementptr inbounds (["
      << (module_name.size() + 1u) << " x i8], ptr " << module_name_symbol
      << ", i32 0, i32 0), ptr getelementptr inbounds (["
      << (translation_unit_identity_key.size() + 1u)
      << " x i8], ptr " << translation_unit_identity_symbol
      << ", i32 0, i32 0)"
      << ", i64 " << registration_order_ordinal << ", i64 "
      << frontend_metadata
             .runtime_metadata_section_publication_class_descriptor_count
      << ", i64 "
      << frontend_metadata
             .runtime_metadata_section_publication_protocol_descriptor_count
      << ", i64 "
      << frontend_metadata
             .runtime_metadata_section_publication_category_descriptor_count
      << ", i64 "
      << frontend_metadata
             .runtime_metadata_section_publication_property_descriptor_count
      << ", i64 "
      << frontend_metadata.runtime_metadata_section_publication_ivar_descriptor_count
      << " }, align 8\n";
  out << image_local_init_state_symbol
      << " = internal global i8 0, align 1\n";
  out << registration_table_symbol << " = internal constant "
      << Objc3IRRuntimeBootstrapRegistrationTableType() << " { i64 "
      << frontend_metadata
             .runtime_bootstrap_lowering_registration_table_abi_version
      << ", i64 "
      << frontend_metadata
             .runtime_bootstrap_lowering_registration_table_pointer_field_count
      << ", ptr " << image_descriptor_symbol << ", ptr "
      << options.discovery_root_symbol << ", ptr " << options.linker_anchor_symbol
      << ", ptr " << class_section_root_symbol << ", ptr "
      << protocol_section_root_symbol << ", ptr " << category_section_root_symbol
      << ", ptr " << property_section_root_symbol << ", ptr "
      << ivar_section_root_symbol << ", ptr " << selector_pool_symbol
      << ", ptr " << string_pool_symbol << ", ptr "
      << keypath_descriptor_root_symbol << ", ptr "
      << image_local_init_state_symbol << " }, align 8\n";

  if (options.emit_registration_descriptor_image_root) {
    out << image_root_symbol
        << " = internal constant { ptr, ptr, ptr, ptr, ptr }"
        << " { ptr getelementptr inbounds (["
        << (image_root_identifier.size() + 1u) << " x i8], ptr "
        << image_root_name_symbol << ", i32 0, i32 0), ptr getelementptr inbounds (["
        << (module_name.size() + 1u) << " x i8], ptr " << module_name_symbol
        << ", i32 0, i32 0), ptr " << image_descriptor_symbol
        << ", ptr " << registration_table_symbol << ", ptr "
        << options.discovery_root_symbol << " }, section \""
        << Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeBootstrapImageRootLogicalSection)
        << "\", align 8\n";
    out << registration_descriptor_symbol
        << " = internal constant { ptr, ptr, ptr, ptr, ptr, ptr }"
        << " { ptr getelementptr inbounds (["
        << (registration_descriptor_identifier.size() + 1u)
        << " x i8], ptr " << registration_descriptor_name_symbol
        << ", i32 0, i32 0), ptr " << image_root_symbol << ", ptr "
        << image_descriptor_symbol << ", ptr " << registration_table_symbol
        << ", ptr " << options.linker_anchor_symbol << ", ptr "
        << image_local_init_state_symbol << " }, section \""
        << Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeBootstrapRegistrationDescriptorLogicalSection)
        << "\", align 8\n";
  }

  const std::uint32_t global_ctor_priority =
      registration_order_ordinal >
              static_cast<std::uint64_t>(
                  std::numeric_limits<std::uint32_t>::max())
          ? std::numeric_limits<std::uint32_t>::max()
          : static_cast<std::uint32_t>(registration_order_ordinal);
  out << "@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] "
         "[{ i32, ptr, ptr } { i32 "
      << global_ctor_priority << ", ptr " << constructor_root_symbol
      << ", ptr " << registration_table_symbol << " }]\n";

  retained_globals.push_back(image_descriptor_symbol);
  retained_globals.push_back(registration_table_symbol);
  retained_globals.push_back(image_local_init_state_symbol);
  if (options.emit_registration_descriptor_image_root) {
    retained_globals.push_back(image_root_symbol);
    retained_globals.push_back(registration_descriptor_symbol);
  }
}

void EmitObjc3IRRuntimeBootstrapLoweringFunctions(
    const Objc3IRFrontendMetadata &frontend_metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::ostringstream &out) {
  if (!Objc3IRRuntimeBootstrapLoweringReady(frontend_metadata)) {
    return;
  }

  const std::string init_stub_symbol =
      "@" + runtime_metadata_symbols.init_stub_symbol;
  const std::string ctor_root_symbol =
      "@" + frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol;
  const std::string registration_table_symbol =
      "@" + runtime_metadata_symbols.registration_table_symbol;
  const std::string register_image_symbol =
      "@" + frontend_metadata
                .runtime_bootstrap_lowering_registration_entrypoint_symbol;

  out << "define internal void " << init_stub_symbol << "() {\n";
  out << "entry:\n";
  out << "  %bootstrap_state_slot = getelementptr inbounds "
      << Objc3IRRuntimeBootstrapRegistrationTableType() << ", ptr "
      << registration_table_symbol << ", i32 0, i32 13\n";
  out << "  %bootstrap_state_cell = load ptr, ptr %bootstrap_state_slot, align 8\n";
  out << "  %bootstrap_state = load i8, ptr %bootstrap_state_cell, align 1\n";
  out << "  %bootstrap_already_initialized = icmp ne i8 %bootstrap_state, 0\n";
  out << "  br i1 %bootstrap_already_initialized, label %bootstrap_success, label %bootstrap_register\n";
  out << "bootstrap_register:\n";
  out << "  call void @" << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
      << "(ptr " << registration_table_symbol << ")\n";
  out << "  %bootstrap_image_slot = getelementptr inbounds "
      << Objc3IRRuntimeBootstrapRegistrationTableType() << ", ptr "
      << registration_table_symbol << ", i32 0, i32 2\n";
  out << "  %bootstrap_image = load ptr, ptr %bootstrap_image_slot, align 8\n";
  out << "  %bootstrap_status = call i32 " << register_image_symbol
      << "(ptr %bootstrap_image)\n";
  out << "  %bootstrap_ok = icmp eq i32 %bootstrap_status, 0\n";
  out << "  br i1 %bootstrap_ok, label %bootstrap_success, label %bootstrap_fail\n";
  out << "bootstrap_fail:\n";
  out << "  call void @abort()\n";
  out << "  unreachable\n";
  out << "bootstrap_success:\n";
  out << "  store i8 1, ptr %bootstrap_state_cell, align 1\n";
  out << "  ret void\n";
  out << "}\n\n";

  out << "define internal void " << ctor_root_symbol << "() {\n";
  out << "entry:\n";
  out << "  call void " << init_stub_symbol << "()\n";
  out << "  ret void\n";
  out << "}\n\n";
}
