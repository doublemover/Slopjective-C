#include "driver/objc3_driver_runtime_import_artifact_readiness.h"

#include "pipeline/objc3_runtime_import_surface.h"

Objc3DriverRuntimeImportArtifactReadiness
CheckObjc3DriverRuntimeImportArtifactReadiness(
    const Objc3FrontendArtifactBundle &artifacts) {
  Objc3DriverRuntimeImportArtifactReadiness readiness;
  readiness.has_runtime_import_artifact =
      !artifacts.runtime_aware_import_module_artifact_json.empty();
  readiness.ready =
      !readiness.has_runtime_import_artifact ||
      IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          artifacts.runtime_aware_import_module_frontend_closure_summary);
  return readiness;
}
