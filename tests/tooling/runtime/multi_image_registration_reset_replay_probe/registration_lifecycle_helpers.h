#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_REGISTRATION_LIFECYCLE_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_REGISTRATION_LIFECYCLE_HELPERS_H_

#include "image_fixture_setup.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::multi_image_registration_reset_replay {

struct RegistrationState {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
};

struct ImageWalkState {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int copy_status = 0;
  std::string last_walked_module_name;
};

struct RealizedClassGraphState {
  objc3_runtime_realized_class_graph_state_snapshot snapshot{};
  int copy_status = 0;
};

struct RealizedClassEntryState {
  objc3_runtime_realized_class_entry_snapshot snapshot{};
  int copy_status = 0;
  std::string translation_unit_identity_key;
};

struct ResetReplayState {
  objc3_runtime_reset_replay_state_snapshot snapshot{};
  int copy_status = 0;
  std::string last_replayed_module_name;
};

struct RuntimeImageLifecycleState {
  RegistrationState registration;
  ImageWalkState walk;
  RealizedClassGraphState graph;
  RealizedClassEntryState provider_entry;
  RealizedClassEntryState consumer_entry;
  ResetReplayState replay;
};

struct PostResetLifecycleState {
  RegistrationState registration;
  ResetReplayState replay;
};

inline RegistrationState CaptureRegistrationState() {
  RegistrationState state;
  state.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.snapshot);
  return state;
}

inline ImageWalkState CaptureImageWalkState() {
  ImageWalkState state;
  state.copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&state.snapshot);
  state.last_walked_module_name =
      CopyRuntimeString(state.snapshot.last_walked_module_name);
  return state;
}

inline RealizedClassGraphState CaptureRealizedClassGraphState() {
  RealizedClassGraphState state;
  state.copy_status =
      objc3_runtime_copy_realized_class_graph_state_for_testing(
          &state.snapshot);
  return state;
}

inline RealizedClassEntryState CaptureRealizedClassEntryState(
    const char *class_name) {
  RealizedClassEntryState state;
  state.copy_status = objc3_runtime_copy_realized_class_entry_for_testing(
      class_name, &state.snapshot);
  state.translation_unit_identity_key =
      CopyRuntimeString(state.snapshot.translation_unit_identity_key);
  return state;
}

inline ResetReplayState CaptureResetReplayState() {
  ResetReplayState state;
  state.copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&state.snapshot);
  state.last_replayed_module_name =
      CopyRuntimeString(state.snapshot.last_replayed_module_name);
  return state;
}

inline RuntimeImageLifecycleState CaptureRuntimeImageLifecycleState() {
  return {
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureRealizedClassGraphState(),
      CaptureRealizedClassEntryState(kImportedProviderClassName),
      CaptureRealizedClassEntryState(kLocalConsumerClassName),
      CaptureResetReplayState(),
  };
}

inline PostResetLifecycleState CapturePostResetLifecycleState() {
  return {
      CaptureRegistrationState(),
      CaptureResetReplayState(),
  };
}

}  // namespace objc3c::runtime::probe::multi_image_registration_reset_replay

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_REGISTRATION_LIFECYCLE_HELPERS_H_
