#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendInteropMetadata {
  std::string lowering_interop_interop_replay_key;
  std::size_t interop_interop_lowering_foreign_callable_sites = 0;
  std::size_t interop_interop_lowering_c_foreign_callable_sites = 0;
  std::size_t interop_interop_lowering_objc_runtime_parity_callable_sites = 0;
  std::size_t interop_interop_lowering_ownership_bridge_callable_sites = 0;
  std::size_t interop_interop_lowering_error_surface_sites = 0;
  std::size_t interop_interop_lowering_async_boundary_sites = 0;
  std::size_t interop_interop_lowering_swift_concurrency_metadata_sites = 0;
  std::size_t interop_interop_lowering_interface_preserved_foreign_callable_sites = 0;
  std::size_t interop_interop_lowering_interface_preserved_metadata_annotation_sites = 0;
  std::size_t interop_interop_lowering_guard_blocked_sites = 0;
  std::size_t interop_interop_lowering_contract_violation_sites = 0;
  bool deterministic_interop_interop_lowering_handoff = false;
  std::string lowering_interop_foreign_call_lifetime_replay_key;
  std::size_t interop_foreign_call_lifetime_lowering_foreign_callable_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_c_foreign_callable_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_objc_runtime_parity_callable_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_ownership_bridge_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_lifetime_bridge_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_metadata_preservation_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_guard_blocked_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_contract_violation_sites = 0;
  bool deterministic_interop_foreign_call_lifetime_lowering_handoff = false;
  std::string lowering_interop_ffi_metadata_interface_preservation_key;
  std::size_t interop_ffi_metadata_interface_preservation_local_foreign_callable_count = 0;
  std::size_t interop_ffi_metadata_interface_preservation_local_metadata_preservation_sites = 0;
  std::size_t interop_ffi_metadata_interface_preservation_local_interface_annotation_sites = 0;
  std::size_t interop_ffi_metadata_interface_preservation_imported_module_count = 0;
  std::size_t interop_ffi_metadata_interface_preservation_imported_foreign_callable_count = 0;
  std::size_t interop_ffi_metadata_interface_preservation_imported_metadata_preservation_sites = 0;
  std::size_t interop_ffi_metadata_interface_preservation_imported_interface_annotation_sites = 0;
  bool interop_ffi_metadata_interface_preservation_runtime_import_artifact_ready = false;
  bool interop_ffi_metadata_interface_preservation_separate_compilation_preservation_ready = false;
  bool deterministic_interop_ffi_metadata_interface_preservation_handoff = false;
  std::string lowering_interop_header_module_bridge_generation_key;
};
