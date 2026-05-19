#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_ORCHESTRATION_H_

#include "error_report_helpers.h"
#include "fixture_runtime_bootstrap.h"
#include "installation_state_helpers.h"
#include "loader_lifecycle_assertions.h"
#include "runtime/public/objc3_runtime_api.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::installation_loader_lifecycle_probe {

inline int RunProbe() {
  const StartupState startup = CaptureStartupState();
  const RegistrationAttemptResult duplicate =
      RunDuplicateRegistrationAttempt(startup);
  const RegistrationAttemptResult out_of_order =
      RunOutOfOrderRegistrationAttempt(startup);

  objc3_runtime_reset_for_testing();
  const PostResetState post_reset = CapturePostResetState();

  FixtureRuntimeBootstrap fixture;
  objc3_runtime_registration_table invalid_anchor_table =
      fixture.MakeInvalidAnchorTable();
  const RegistrationAttemptResult invalid_anchor =
      RunStagedRegistrationAttempt(fixture.compiled_image_descriptor,
                                   &invalid_anchor_table);

  objc3_runtime_registration_table invalid_discovery_root_table =
      fixture.MakeInvalidDiscoveryRootTable();
  const RegistrationAttemptResult invalid_discovery_root =
      RunStagedRegistrationAttempt(fixture.compiled_image_descriptor,
                                   &invalid_discovery_root_table);

  const int replay_status = objc3_runtime_replay_registered_images_for_testing();
  const PostReplayState post_replay = CapturePostReplayState();

  PrintProbeReport(startup, duplicate, out_of_order, post_reset, invalid_anchor,
                   invalid_discovery_root, replay_status, post_replay);
  return 0;
}

}  // namespace objc3c::runtime::installation_loader_lifecycle_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_ORCHESTRATION_H_
