#include "artifacts/objc3_frontend_artifact_manifest_readiness.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifacts.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestParseReadiness(
    std::ostream &manifest,
    const Objc3FrontendArtifactBundle &bundle) {
  const auto &readiness_surface = bundle.parse_lowering_readiness_surface;

  manifest << "      \"parse_lowering_readiness\": {";
#include "objc3_frontend_artifact_manifest_readiness_gate_fields.inc"
#include "objc3_frontend_artifact_manifest_readiness_stage_fields.inc"
#include "objc3_frontend_artifact_manifest_readiness_status_fields.inc"
#include "objc3_frontend_artifact_manifest_readiness_replay_key_failure_fields.inc"
}

}  // namespace objc3::artifacts::frontend
