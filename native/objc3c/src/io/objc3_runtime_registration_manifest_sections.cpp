#include "io/objc3_runtime_registration_manifest_sections.h"

#include "io/objc3_json.h"

void AppendObjc3RuntimeRegistrationManifestAccessorAbiSurfacesJson(
    std::ostream &out,
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
        &inputs) {
  out << "  \"dispatch_accessor_runtime_abi_surface\": {\n"
      << "    \"contract_id\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_runtime_abi_contract_id)
      << "\",\n"
      << "    \"abi_boundary_model\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_runtime_abi_boundary_model)
      << "\",\n"
      << "    \"public_header_path\": \""
      << objc3::io::EscapeJsonString(inputs.dispatch_accessor_public_header_path)
      << "\",\n"
      << "    \"private_header_path\": \""
      << objc3::io::EscapeJsonString(inputs.dispatch_accessor_private_header_path)
      << "\",\n"
      << "    \"runtime_dispatch_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_runtime_dispatch_symbol)
      << "\",\n"
      << "    \"dispatch_state_snapshot_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_dispatch_state_snapshot_symbol)
      << "\",\n"
      << "    \"method_cache_state_snapshot_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_method_cache_state_snapshot_symbol)
      << "\",\n"
      << "    \"method_cache_entry_snapshot_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_method_cache_entry_snapshot_symbol)
      << "\",\n"
      << "    \"property_registry_state_snapshot_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_property_registry_state_snapshot_symbol)
      << "\",\n"
      << "    \"property_entry_snapshot_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_property_entry_snapshot_symbol)
      << "\",\n"
      << "    \"arc_debug_state_snapshot_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_arc_debug_state_snapshot_symbol)
      << "\",\n"
      << "    \"current_property_read_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_current_property_read_symbol)
      << "\",\n"
      << "    \"current_property_write_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_current_property_write_symbol)
      << "\",\n"
      << "    \"current_property_exchange_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_current_property_exchange_symbol)
      << "\",\n"
      << "    \"bind_current_property_context_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_bind_current_property_context_symbol)
      << "\",\n"
      << "    \"clear_current_property_context_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_clear_current_property_context_symbol)
      << "\",\n"
      << "    \"weak_current_property_load_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_weak_current_property_load_symbol)
      << "\",\n"
      << "    \"weak_current_property_store_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.dispatch_accessor_weak_current_property_store_symbol)
      << "\",\n"
      << "    \"retain_symbol\": \""
      << objc3::io::EscapeJsonString(inputs.dispatch_accessor_retain_symbol)
      << "\",\n"
      << "    \"release_symbol\": \""
      << objc3::io::EscapeJsonString(inputs.dispatch_accessor_release_symbol)
      << "\",\n"
      << "    \"autorelease_symbol\": \""
      << objc3::io::EscapeJsonString(inputs.dispatch_accessor_autorelease_symbol)
      << "\",\n"
      << "    \"private_testing_surface_only\": "
      << (inputs.dispatch_accessor_private_testing_surface_only ? "true"
                                                               : "false")
      << ",\n"
      << "    \"deterministic\": "
      << (inputs.dispatch_accessor_deterministic ? "true" : "false") << "\n"
      << "  },\n"
      << "  \"storage_accessor_runtime_abi_surface\": {\n"
      << "    \"contract_id\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_runtime_abi_contract_id)
      << "\",\n"
      << "    \"abi_boundary_model\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_runtime_abi_boundary_model)
      << "\",\n"
      << "    \"public_header_path\": \""
      << objc3::io::EscapeJsonString(inputs.storage_accessor_public_header_path)
      << "\",\n"
      << "    \"private_header_path\": \""
      << objc3::io::EscapeJsonString(inputs.storage_accessor_private_header_path)
      << "\",\n"
      << "    \"property_registry_state_snapshot_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_property_registry_state_snapshot_symbol)
      << "\",\n"
      << "    \"property_entry_snapshot_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_property_entry_snapshot_symbol)
      << "\",\n"
      << "    \"current_property_read_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_current_property_read_symbol)
      << "\",\n"
      << "    \"current_property_write_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_current_property_write_symbol)
      << "\",\n"
      << "    \"current_property_exchange_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_current_property_exchange_symbol)
      << "\",\n"
      << "    \"bind_current_property_context_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_bind_current_property_context_symbol)
      << "\",\n"
      << "    \"clear_current_property_context_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_clear_current_property_context_symbol)
      << "\",\n"
      << "    \"weak_current_property_load_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_weak_current_property_load_symbol)
      << "\",\n"
      << "    \"weak_current_property_store_symbol\": \""
      << objc3::io::EscapeJsonString(
             inputs.storage_accessor_weak_current_property_store_symbol)
      << "\",\n"
      << "    \"private_testing_surface_only\": "
      << (inputs.storage_accessor_private_testing_surface_only ? "true"
                                                              : "false")
      << ",\n"
      << "    \"deterministic\": "
      << (inputs.storage_accessor_deterministic ? "true" : "false") << "\n"
      << "  },\n";
}
