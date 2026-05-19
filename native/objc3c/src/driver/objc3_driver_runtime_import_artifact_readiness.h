#pragma once

#include "artifacts/objc3_frontend_artifacts.h"

struct Objc3DriverRuntimeImportArtifactReadiness {
  bool has_runtime_import_artifact = false;
  bool ready = false;
};

Objc3DriverRuntimeImportArtifactReadiness
CheckObjc3DriverRuntimeImportArtifactReadiness(
    const Objc3FrontendArtifactBundle &artifacts);
