#include "pipeline/runtime_import_manifest_preservation.h"

namespace objc3c::pipeline {

bool ValidateImportedRuntimeRegistrationManifestReadiness(
    bool ready_for_runtime_bootstrap_enforcement,
    bool ready_for_live_registration_discovery_replay,
    bool ready_for_live_restart_hardening,
    std::string &error) {
  if (!ready_for_runtime_bootstrap_enforcement) {
    error =
        "runtime registration manifest is not ready for runtime bootstrap enforcement";
    return false;
  }
  if (!ready_for_live_registration_discovery_replay) {
    error =
        "runtime registration manifest is not ready for live registration discovery replay";
    return false;
  }
  if (!ready_for_live_restart_hardening) {
    error =
        "runtime registration manifest is not ready for live restart hardening";
    return false;
  }
  return true;
}

}  // namespace objc3c::pipeline
