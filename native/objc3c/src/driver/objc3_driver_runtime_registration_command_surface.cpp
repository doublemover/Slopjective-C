#include "driver/objc3_driver_runtime_registration_command_surface.h"

#include "ast/objc3_ast.h"
#include "io/objc3_process.h"
#include "runtime/metadata/selector_metadata.h"

void PopulateObjc3DriverRuntimeRegistrationCommandSurfaceInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  inputs.compile_wrapper_command_surface =
      summary.compile_wrapper_command_surface;
  inputs.compile_proof_command_surface = summary.compile_proof_command_surface;
  inputs.execution_smoke_command_surface =
      summary.execution_smoke_command_surface;
}
