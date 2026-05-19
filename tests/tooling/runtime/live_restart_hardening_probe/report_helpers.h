#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_REPORT_HELPERS_H_

#include "probe_state.h"
#include "../support/json_probe_writer.h"

#include <cstdint>
#include <cstdio>

namespace objc3c::runtime::live_restart_hardening_probe {

inline unsigned long long JsonU64(std::uint64_t value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintStartupReportFields(const BootstrapState &startup) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("startup_registration_copy_status",
                startup.registration.copy_status);
  PrintIntField("startup_reset_replay_copy_status",
                startup.reset_replay.copy_status);
  PrintUint64Field(
      "startup_registered_image_count",
      JsonU64(startup.registration.snapshot.registered_image_count));
  PrintStringField(
      "startup_last_registered_translation_unit_identity_key",
      startup.registration.snapshot.last_registered_translation_unit_identity_key);
}

inline void PrintUnsupportedReplayReportFields(
    bool second_attempt,
    const UnsupportedReplayState &unsupported_replay) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  if (!second_attempt) {
    PrintIntField("unsupported_replay_status",
                  unsupported_replay.replay_status);
    PrintIntField("unsupported_replay_registration_copy_status",
                  unsupported_replay.registration.copy_status);
    PrintIntField("unsupported_replay_reset_replay_copy_status",
                  unsupported_replay.reset_replay.copy_status);
    PrintUint64Field(
        "unsupported_replay_registered_image_count",
        JsonU64(
            unsupported_replay.registration.snapshot.registered_image_count));
    PrintIntField("unsupported_replay_last_replay_status",
                  unsupported_replay.reset_replay.snapshot.last_replay_status);
    PrintUint64Field(
        "unsupported_replay_last_replayed_image_count",
        JsonU64(unsupported_replay.reset_replay.snapshot
                    .last_replayed_image_count));
    return;
  }

  PrintIntField("second_unsupported_replay_status",
                unsupported_replay.replay_status);
  PrintIntField("second_unsupported_replay_registration_copy_status",
                unsupported_replay.registration.copy_status);
  PrintIntField("second_unsupported_replay_reset_replay_copy_status",
                unsupported_replay.reset_replay.copy_status);
  PrintUint64Field(
      "second_unsupported_replay_registered_image_count",
      JsonU64(unsupported_replay.registration.snapshot.registered_image_count));
  PrintIntField("second_unsupported_replay_last_replay_status",
                unsupported_replay.reset_replay.snapshot.last_replay_status);
}

inline void PrintResetReportFields(bool second_cycle,
                                   const ResetState &reset) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  if (!second_cycle) {
    PrintIntField("post_reset_registration_copy_status",
                  reset.registration.copy_status);
    PrintIntField("post_reset_reset_replay_copy_status",
                  reset.reset_replay.copy_status);
    PrintUint64Field(
        "post_reset_registered_image_count",
        JsonU64(reset.registration.snapshot.registered_image_count));
    PrintUint64Field(
        "post_reset_next_expected_registration_order_ordinal",
        JsonU64(reset.registration.snapshot
                    .next_expected_registration_order_ordinal));
    PrintUint64Field(
        "post_reset_retained_bootstrap_image_count",
        JsonU64(reset.reset_replay.snapshot.retained_bootstrap_image_count));
    PrintUint64Field(
        "post_reset_last_reset_cleared_image_local_init_state_count",
        JsonU64(reset.reset_replay.snapshot
                    .last_reset_cleared_image_local_init_state_count));
    PrintUint64Field("post_reset_reset_generation",
                     JsonU64(reset.reset_replay.snapshot.reset_generation));
    return;
  }

  PrintIntField("second_reset_registration_copy_status",
                reset.registration.copy_status);
  PrintIntField("second_reset_reset_replay_copy_status",
                reset.reset_replay.copy_status);
  PrintUint64Field(
      "second_reset_registered_image_count",
      JsonU64(reset.registration.snapshot.registered_image_count));
  PrintUint64Field(
      "second_reset_next_expected_registration_order_ordinal",
      JsonU64(
          reset.registration.snapshot.next_expected_registration_order_ordinal));
  PrintUint64Field(
      "second_reset_retained_bootstrap_image_count",
      JsonU64(reset.reset_replay.snapshot.retained_bootstrap_image_count));
  PrintUint64Field(
      "second_reset_last_reset_cleared_image_local_init_state_count",
      JsonU64(reset.reset_replay.snapshot
                  .last_reset_cleared_image_local_init_state_count));
  PrintUint64Field("second_reset_reset_generation",
                   JsonU64(reset.reset_replay.snapshot.reset_generation));
}

inline void PrintRestartReportFields(bool second_cycle,
                                     const RestartState &restart,
                                     bool trailing_comma = true) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  if (!second_cycle) {
    PrintIntField("first_restart_status", restart.replay_status);
    PrintIntField("first_restart_registration_copy_status",
                  restart.registration.copy_status);
    PrintIntField("first_restart_image_walk_copy_status",
                  restart.image_walk.copy_status);
    PrintIntField("first_restart_reset_replay_copy_status",
                  restart.reset_replay.copy_status);
    PrintUint64Field(
        "first_restart_registered_image_count",
        JsonU64(restart.registration.snapshot.registered_image_count));
    PrintIntField("first_restart_last_registration_status",
                  restart.registration.snapshot.last_registration_status);
    PrintUint64Field(
        "first_restart_last_replayed_image_count",
        JsonU64(
            restart.reset_replay.snapshot.last_replayed_image_count));
    PrintUint64Field("first_restart_replay_generation",
                     JsonU64(restart.reset_replay.snapshot.replay_generation));
    PrintStringField(
        "first_restart_last_registered_translation_unit_identity_key",
        restart.registration.snapshot.last_registered_translation_unit_identity_key);
    PrintStringField(
        "first_restart_last_replayed_translation_unit_identity_key",
        restart.reset_replay.snapshot
            .last_replayed_translation_unit_identity_key);
    PrintStringField(
        "first_restart_last_walked_translation_unit_identity_key",
        restart.image_walk.snapshot.last_walked_translation_unit_identity_key);
    return;
  }

  PrintIntField("second_restart_status", restart.replay_status);
  PrintIntField("second_restart_registration_copy_status",
                restart.registration.copy_status);
  PrintIntField("second_restart_image_walk_copy_status",
                restart.image_walk.copy_status);
  PrintIntField("second_restart_reset_replay_copy_status",
                restart.reset_replay.copy_status);
  PrintUint64Field(
      "second_restart_registered_image_count",
      JsonU64(restart.registration.snapshot.registered_image_count));
  PrintIntField("second_restart_last_registration_status",
                restart.registration.snapshot.last_registration_status);
  PrintUint64Field(
      "second_restart_last_replayed_image_count",
      JsonU64(restart.reset_replay.snapshot.last_replayed_image_count));
  PrintUint64Field("second_restart_replay_generation",
                   JsonU64(restart.reset_replay.snapshot.replay_generation));
  PrintStringField(
      "second_restart_last_registered_translation_unit_identity_key",
      restart.registration.snapshot.last_registered_translation_unit_identity_key);
  PrintStringField(
      "second_restart_last_replayed_translation_unit_identity_key",
      restart.reset_replay.snapshot.last_replayed_translation_unit_identity_key);
  PrintStringField(
      "second_restart_last_walked_translation_unit_identity_key",
      restart.image_walk.snapshot.last_walked_translation_unit_identity_key,
      trailing_comma);
}

inline void PrintProbeReport(const ProbeRun &run) {
  std::printf("{");
  PrintStartupReportFields(run.startup);
  PrintUnsupportedReplayReportFields(false, run.unsupported_replay);
  PrintResetReportFields(false, run.first_cycle.reset);
  PrintRestartReportFields(false, run.first_cycle.restart);
  PrintUnsupportedReplayReportFields(true, run.second_unsupported_replay);
  PrintResetReportFields(true, run.second_cycle.reset);
  PrintRestartReportFields(true, run.second_cycle.restart, false);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::live_restart_hardening_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_REPORT_HELPERS_H_
