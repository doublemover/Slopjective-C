#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_PROBE_RESULT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_PROBE_RESULT_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::cross_module_runtime_packaging {

inline constexpr const char *kImportedProviderClassName = "ImportedProvider";
inline constexpr const char *kLocalConsumerClassName = "LocalConsumer";
inline constexpr const char *kImportedWorkerProtocolName = "ImportedWorker";
inline constexpr const char *kProviderClassValueSelector =
    "providerClassValue";
inline constexpr const char *kImportedProtocolValueSelector =
    "importedProtocolValue";
inline constexpr const char *kLocalClassValueSelector = "localClassValue";

inline std::string CopyRuntimeCString(const char *value) {
  return value != nullptr ? value : "";
}

inline const char *NullableCopiedCString(const std::string &value) {
  return value.empty() ? nullptr : value.c_str();
}

inline int RuntimeClassReceiver(
    const objc3_runtime_realized_class_entry_snapshot &entry) {
  return entry.found != 0 ? static_cast<int>(entry.base_identity + 2U) : 0;
}

struct StartupPackageModuleState {
  objc3_runtime_registration_state_snapshot registration{};
  objc3_runtime_realized_class_entry_snapshot imported_entry{};
  objc3_runtime_realized_class_entry_snapshot local_entry{};
  int registration_copy_status = 0;
  int imported_entry_status = 0;
  int local_entry_status = 0;
  std::string last_registered_translation_unit_identity_key;

  int ImportedClassReceiver() const {
    return RuntimeClassReceiver(imported_entry);
  }

  int LocalClassReceiver() const {
    return RuntimeClassReceiver(local_entry);
  }
};

struct CrossModuleRuntimeDispatchValues {
  int imported_provider_class_value = 0;
  int imported_provider_protocol_value = 0;
  int local_consumer_class_value = 0;
};

struct CrossModuleRuntimeAssertions {
  CrossModuleRuntimeDispatchValues dispatch_values;
  objc3_runtime_protocol_conformance_query_snapshot imported_worker_query{};
  int imported_worker_query_status = 0;
};

struct PostResetRuntimeState {
  objc3_runtime_registration_state_snapshot registration{};
  objc3_runtime_reset_replay_state_snapshot replay{};
  int registration_copy_status = 0;
  int replay_copy_status = 0;
};

struct PostReplayPackageModuleState {
  objc3_runtime_registration_state_snapshot registration{};
  objc3_runtime_reset_replay_state_snapshot replay{};
  objc3_runtime_realized_class_entry_snapshot imported_entry{};
  objc3_runtime_realized_class_entry_snapshot local_entry{};
  int registration_copy_status = 0;
  int replay_copy_status = 0;
  int imported_entry_status = 0;
  int local_entry_status = 0;
  std::string last_registered_translation_unit_identity_key;
  std::string last_replayed_translation_unit_identity_key;

  int ImportedClassReceiver() const {
    return RuntimeClassReceiver(imported_entry);
  }

  int LocalClassReceiver() const {
    return RuntimeClassReceiver(local_entry);
  }
};

struct ProbeResult {
  StartupPackageModuleState startup;
  CrossModuleRuntimeAssertions startup_assertions;
  PostResetRuntimeState post_reset;
  int replay_status = 0;
  PostReplayPackageModuleState post_replay;
  CrossModuleRuntimeDispatchValues post_replay_dispatch_values;
};

}  // namespace objc3c::runtime::probe::cross_module_runtime_packaging

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_PROBE_RESULT_H_
