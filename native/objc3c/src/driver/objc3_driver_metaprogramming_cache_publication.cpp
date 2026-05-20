#include "driver/objc3_driver_metaprogramming_cache_publication.h"

#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"

int PublishObjc3DriverMetaprogrammingCacheArtifact(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    std::string &error_message) {
  error_message.clear();
  if (!artifacts
           .metaprogramming_macro_host_process_cache_runtime_integration_ready) {
    return Objc3DriverStatusValue(Objc3DriverStatusCode::kSuccess);
  }

  std::string artifact_json;
  if (!TryBuildObjc3MetaprogrammingMacroHostProcessCacheArtifact(
          {.contract_id =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId,
           .source_contract_id =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId,
           .surface_path =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath,
           .import_artifact_member_name =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName,
           .artifact_relative_path =
               kObjc3MetaprogrammingMacroHostProcessCacheArtifactRelativePath,
           .host_executable_relative_path =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath,
           .cache_root_relative_path =
               artifacts
                   .metaprogramming_macro_host_process_cache_runtime_integration_cache_root_relative_path,
           .host_model =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel,
           .toolchain_model =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel,
           .cache_model =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel,
           .invalidation_model =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationInvalidationModel,
           .sandbox_policy_model =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSandboxPolicyModel,
           .diagnostics_model =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationDiagnosticsModel,
           .fail_closed_model =
               kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel,
           .replay_key =
               artifacts
                   .metaprogramming_macro_host_process_cache_runtime_integration_replay_key,
           .runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol,
           .max_message_send_args =
               static_cast<std::uint32_t>(cli_options.max_message_send_args),
           .bootstrap_registration_order_ordinal =
               cli_options.bootstrap_registration_order_ordinal,
           .allow_live_error_runtime_surface =
               cli_options.allow_live_error_runtime_surface,
           .imported_runtime_surface_paths =
               cli_options.imported_runtime_surface_paths,
           .deterministic = true},
          cli_options.input,
          artifact_json,
          error_message)) {
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }

  WriteMetaprogrammingMacroHostProcessCacheArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      artifact_json);
  return Objc3DriverStatusValue(Objc3DriverStatusCode::kSuccess);
}
