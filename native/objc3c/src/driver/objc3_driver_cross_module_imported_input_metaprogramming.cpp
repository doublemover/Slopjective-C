#include "driver/objc3_driver_cross_module_imported_input_metaprogramming.h"

void PopulateObjc3DriverCrossModuleRuntimeImportedInputMetaprogramming(
    Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    const Objc3ImportedRuntimeModuleSurface &imported_surface) {
  imported_input
      .metaprogramming_macro_host_process_cache_runtime_integration_present =
      imported_surface
          .metaprogramming_macro_host_process_cache_runtime_integration_present;
  imported_input.metaprogramming_macro_host_process_cache_runtime_ready =
      imported_surface.metaprogramming_macro_host_process_cache_runtime_ready;
  imported_input
      .metaprogramming_macro_host_process_cache_separate_compilation_ready =
      imported_surface
          .metaprogramming_macro_host_process_cache_separate_compilation_ready;
  imported_input.metaprogramming_macro_host_process_cache_deterministic =
      imported_surface.metaprogramming_macro_host_process_cache_deterministic;
  imported_input.metaprogramming_macro_host_process_cache_contract_id =
      imported_surface.metaprogramming_macro_host_process_cache_contract_id;
  imported_input.metaprogramming_macro_host_process_cache_source_contract_id =
      imported_surface
          .metaprogramming_macro_host_process_cache_source_contract_id;
  imported_input.metaprogramming_macro_host_process_cache_replay_key =
      imported_surface.metaprogramming_macro_host_process_cache_replay_key;
  imported_input
      .metaprogramming_macro_host_process_cache_host_executable_relative_path =
      imported_surface
          .metaprogramming_macro_host_process_cache_host_executable_relative_path;
  imported_input.metaprogramming_macro_host_process_cache_root_relative_path =
      imported_surface
          .metaprogramming_macro_host_process_cache_root_relative_path;
  imported_input.metaprogramming_macro_host_process_cache_package_identity =
      imported_surface
          .metaprogramming_macro_host_process_cache_package_identity;
  imported_input
      .metaprogramming_macro_host_process_cache_package_lock_identity =
      imported_surface
          .metaprogramming_macro_host_process_cache_package_lock_identity;
  imported_input
      .metaprogramming_macro_host_process_cache_package_trust_identity =
      imported_surface
          .metaprogramming_macro_host_process_cache_package_trust_identity;
  imported_input
      .metaprogramming_macro_host_process_cache_input_content_identity =
      imported_surface
          .metaprogramming_macro_host_process_cache_input_content_identity;
  imported_input
      .metaprogramming_macro_host_process_cache_output_content_identity =
      imported_surface
          .metaprogramming_macro_host_process_cache_output_content_identity;
  imported_input.metaprogramming_macro_host_process_cache_host_identity =
      imported_surface.metaprogramming_macro_host_process_cache_host_identity;
  imported_input.metaprogramming_macro_host_process_cache_validation_status =
      imported_surface
          .metaprogramming_macro_host_process_cache_validation_status;
  imported_input
      .metaprogramming_macro_host_process_cache_runtime_consumption_artifact_identity =
      imported_surface
          .metaprogramming_macro_host_process_cache_runtime_consumption_artifact_identity;
  imported_input
      .metaprogramming_macro_host_process_cache_package_replay_generation =
      imported_surface
          .metaprogramming_macro_host_process_cache_package_replay_generation;
}
