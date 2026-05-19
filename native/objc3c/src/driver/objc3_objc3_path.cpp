#include "driver/objc3_objc3_path.h"

#include <exception>
#include <string>

#include "driver/objc3_driver_conformance_publication.h"
#include "driver/objc3_driver_conformance_surface.h"
#include "driver/objc3_driver_cross_module_link_publication.h"
#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_frontend_artifact_handoff.h"
#include "driver/objc3_driver_metaprogramming_cache_publication.h"
#include "driver/objc3_driver_object_backend.h"
#include "driver/objc3_driver_runtime_registration_publication.h"
#include "driver/objc3_driver_status_codes.h"
#include "driver/objc3_driver_toolchain_runtime_gate.h"
#include "driver/objc3_frontend_options.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_file_io.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

int RunObjc3LanguagePath(const Objc3CliOptions &cli_options) {
  try {
    std::string retired_claim_sidecar_error;
    if (!DiagnoseObjc3RetiredClaimSidecars(
            cli_options.out_dir,
            cli_options.emit_prefix,
            retired_claim_sidecar_error)) {
      EmitObjc3DriverError(retired_claim_sidecar_error);
      return Objc3DriverStatusValue(
          Objc3DriverStatusCode::kHardCutoverContractFailure);
    }

    std::string conformance_selection_error;
    if (!ValidateObjc3DriverConformanceSelection(
            cli_options, conformance_selection_error)) {
      EmitObjc3DriverError(conformance_selection_error);
      return Objc3DriverStatusValue(
          Objc3DriverStatusCode::kHardCutoverContractFailure);
    }

    const std::string source = ReadText(cli_options.input);
    const Objc3FrontendOptions frontend_options =
        BuildObjc3FrontendOptions(cli_options);
    Objc3FrontendArtifactBundle artifacts =
        CompileObjc3SourceForCli(cli_options.input, source, frontend_options);

    const Objc3DriverFrontendArtifactHandoffResult artifact_handoff =
        PublishObjc3DriverFrontendArtifactHandoff(cli_options, artifacts);
    if (artifact_handoff.status_code != 0) {
      return artifact_handoff.status_code;
    }

    std::string metaprogramming_cache_error;
    const int metaprogramming_cache_status =
        PublishObjc3DriverMetaprogrammingCacheArtifact(
            cli_options, artifacts, metaprogramming_cache_error);
    if (metaprogramming_cache_status != 0) {
      EmitObjc3DriverError(metaprogramming_cache_error);
      return metaprogramming_cache_status;
    }

    const int conformance_publication_status =
        PublishObjc3DriverConformanceArtifacts(cli_options, artifacts);
    if (conformance_publication_status != 0) {
      return conformance_publication_status;
    }

    const Objc3DriverObjectBackendResult object_backend =
        EmitObjc3DriverObjectBackend(cli_options, artifacts.ir_text);
    if (object_backend.status_code != 0) {
      return object_backend.status_code;
    }

    const Objc3DriverRuntimeRegistrationPublicationResult
        runtime_registration =
            PublishObjc3DriverRuntimeRegistrationArtifacts(
                cli_options, artifacts, object_backend);
    int compile_status = runtime_registration.compile_status;
    if (compile_status == 0 && runtime_registration.linker_retention_ready) {
      compile_status = PublishObjc3DriverCrossModuleRuntimeLinkArtifacts(
          cli_options,
          artifacts,
          runtime_registration.linker_retention_artifacts,
          object_backend.object_out);
    }

    std::string toolchain_runtime_core_feature_reason;
    if (!ValidateObjc3DriverToolchainRuntimeCoreFeature(
            object_backend,
            compile_status,
            toolchain_runtime_core_feature_reason)) {
      EmitObjc3DriverError(
          "toolchain/runtime core feature fail-closed: " +
          toolchain_runtime_core_feature_reason);
      return Objc3DriverStatusValue(
          Objc3DriverStatusCode::kNativeToolchainFailure);
    }
    return Objc3DriverStatusValue(Objc3DriverStatusCode::kSuccess);
  } catch (const std::exception &io_error) {
    EmitObjc3DriverError(std::string("artifact io failure: ") +
                         io_error.what());
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kNativeToolchainFailure);
  }
}
