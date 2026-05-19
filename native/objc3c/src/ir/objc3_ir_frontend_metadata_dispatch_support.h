#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendDispatchSupportMetadata {
  std::string lowering_dispatch_dispatch_control_replay_key;
  std::size_t dispatch_dispatch_control_lowering_direct_call_candidate_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_direct_members_defaulted_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_dynamic_opt_out_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_final_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_sealed_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_override_legality_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_metadata_preserved_callable_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_metadata_preserved_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_guard_blocked_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_contract_violation_sites = 0;
  bool deterministic_dispatch_dispatch_control_lowering_handoff = false;
  std::string lowering_dispatch_dispatch_metadata_interface_preservation_key;
  std::size_t dispatch_dispatch_metadata_local_direct_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_final_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_final_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_sealed_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_module_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_direct_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_final_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_final_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_sealed_container_record_count = 0;
  bool dispatch_dispatch_metadata_runtime_import_artifact_ready = false;
  bool dispatch_dispatch_metadata_separate_compilation_preservation_ready = false;
  bool deterministic_dispatch_dispatch_metadata_interface_handoff = false;
};
