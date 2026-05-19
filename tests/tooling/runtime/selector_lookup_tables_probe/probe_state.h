#pragma once

#include "lookup_assertions.h"
#include "selector_fixtures.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdint>
#include <string>

namespace objc3c {
namespace runtime {
namespace probe {
namespace selector_lookup_tables {

struct RegistrationObservation {
  objc3_runtime_registration_state_snapshot snapshot{};
  int status = 0;
  std::string module_storage;
  std::string identity_storage;
  std::string rejected_module_storage;
  std::string rejected_identity_storage;
};

struct ImageWalkObservation {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int status = 0;
  std::string module_storage;
  std::string identity_storage;
};

struct SelectorTableObservation {
  objc3_runtime_selector_lookup_table_state_snapshot snapshot{};
  int status = 0;
  std::string selector_storage;
};

struct SelectorEntryObservation {
  objc3_runtime_selector_lookup_entry_snapshot snapshot{};
  int status = 0;
  std::string selector_storage;
};

struct ResetReplayObservation {
  objc3_runtime_reset_replay_state_snapshot snapshot{};
  int status = 0;
  std::string module_storage;
  std::string identity_storage;
};

struct StartupObservations {
  RegistrationObservation registration;
  ImageWalkObservation image_walk;
  SelectorTableObservation table;
  SelectorEntryObservation token;
  SelectorEntryObservation current;
  SelectorEntryObservation set_current;
  SelectorEntryObservation shared;
  SelectorEntryObservation manual_only_before;
};

struct AfterManualObservations {
  RegistrationObservation registration;
  ImageWalkObservation image_walk;
  SelectorTableObservation table;
  SelectorEntryObservation token;
  SelectorEntryObservation debug_name;
};

struct AfterDynamicObservations {
  SelectorTableObservation table;
  SelectorEntryObservation manual_only;
};

struct AfterResetObservations {
  RegistrationObservation registration;
  SelectorTableObservation table;
};

struct AfterReplayObservations {
  RegistrationObservation registration;
  ImageWalkObservation image_walk;
  ResetReplayObservation reset_replay;
  SelectorTableObservation table;
  SelectorEntryObservation token;
  SelectorEntryObservation debug_name;
  SelectorEntryObservation manual_only_before;
};

struct AfterReplayDynamicObservations {
  SelectorTableObservation table;
  SelectorEntryObservation manual_only;
};

struct SelectorLookupProbeRun {
  StartupObservations startup;
  int manual_register_status = 0;
  AfterManualObservations after_manual;
  std::uint64_t dynamic_handle_stable_id = 0;
  AfterDynamicObservations after_dynamic;
  AfterResetObservations after_reset;
  int replay_status = 0;
  AfterReplayObservations after_replay;
  std::uint64_t replayed_dynamic_handle_stable_id = 0;
  AfterReplayDynamicObservations after_replay_dynamic;
};

inline void CaptureRegistrationObservation(RegistrationObservation &observation) {
  observation.status =
      objc3_runtime_copy_registration_state_for_testing(&observation.snapshot);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      observation.snapshot, observation.module_storage,
      observation.identity_storage, observation.rejected_module_storage,
      observation.rejected_identity_storage);
}

inline void CaptureImageWalkObservation(ImageWalkObservation &observation) {
  observation.status =
      objc3_runtime_copy_image_walk_state_for_testing(&observation.snapshot);
  ::objc3c::runtime::probe::StabilizeImageWalkState(
      observation.snapshot, observation.module_storage,
      observation.identity_storage);
}

inline void CaptureSelectorTableObservation(
    SelectorTableObservation &observation) {
  observation.status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(
          &observation.snapshot);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      observation.snapshot, observation.selector_storage);
}

inline void CaptureSelectorEntryObservation(
    const char *selector,
    SelectorEntryObservation &observation) {
  observation.status = CopySelectorEntry(selector, &observation.snapshot);
  ::objc3c::runtime::probe::StabilizeSelectorEntry(
      observation.snapshot, observation.selector_storage);
}

inline void CaptureResetReplayObservation(ResetReplayObservation &observation) {
  observation.status =
      objc3_runtime_copy_reset_replay_state_for_testing(&observation.snapshot);
  ::objc3c::runtime::probe::StabilizeResetReplayState(
      observation.snapshot, observation.module_storage,
      observation.identity_storage);
}

inline void CaptureStartupObservations(StartupObservations &startup) {
  CaptureRegistrationObservation(startup.registration);
  CaptureImageWalkObservation(startup.image_walk);
  CaptureSelectorTableObservation(startup.table);
  CaptureSelectorEntryObservation(kManualSelectorTokenValue, startup.token);
  CaptureSelectorEntryObservation(kCurrentValueSelector, startup.current);
  CaptureSelectorEntryObservation(kSetCurrentValueSelector, startup.set_current);
  CaptureSelectorEntryObservation(kSharedSelector, startup.shared);
  CaptureSelectorEntryObservation(kDynamicSelector,
                                  startup.manual_only_before);
}

inline void CaptureAfterManualObservations(
    AfterManualObservations &after_manual) {
  CaptureRegistrationObservation(after_manual.registration);
  CaptureImageWalkObservation(after_manual.image_walk);
  CaptureSelectorTableObservation(after_manual.table);
  CaptureSelectorEntryObservation(kManualSelectorTokenValue,
                                  after_manual.token);
  CaptureSelectorEntryObservation(kManualSelectorDebugName,
                                  after_manual.debug_name);
}

inline void CaptureAfterDynamicObservations(
    AfterDynamicObservations &after_dynamic) {
  CaptureSelectorTableObservation(after_dynamic.table);
  CaptureSelectorEntryObservation(kDynamicSelector, after_dynamic.manual_only);
}

inline void CaptureAfterResetObservations(AfterResetObservations &after_reset) {
  CaptureRegistrationObservation(after_reset.registration);
  CaptureSelectorTableObservation(after_reset.table);
}

inline void CaptureAfterReplayObservations(
    AfterReplayObservations &after_replay) {
  CaptureRegistrationObservation(after_replay.registration);
  CaptureImageWalkObservation(after_replay.image_walk);
  CaptureResetReplayObservation(after_replay.reset_replay);
  CaptureSelectorTableObservation(after_replay.table);
  CaptureSelectorEntryObservation(kManualSelectorTokenValue,
                                  after_replay.token);
  CaptureSelectorEntryObservation(kManualSelectorDebugName,
                                  after_replay.debug_name);
  CaptureSelectorEntryObservation(kDynamicSelector,
                                  after_replay.manual_only_before);
}

inline void CaptureAfterReplayDynamicObservations(
    AfterReplayDynamicObservations &after_replay_dynamic) {
  CaptureSelectorTableObservation(after_replay_dynamic.table);
  CaptureSelectorEntryObservation(kDynamicSelector,
                                  after_replay_dynamic.manual_only);
}

}  // namespace selector_lookup_tables
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
