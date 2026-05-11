#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_PROBE_RESULT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_PROBE_RESULT_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::import_module_execution_matrix {

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

inline int MatrixClassReceiver(
    const objc3_runtime_realized_class_entry_snapshot &entry) {
  return entry.found != 0 ? static_cast<int>(entry.base_identity + 2U) : 0;
}

struct StartupFixtureModuleState {
  objc3_runtime_registration_state_snapshot registration{};
  objc3_runtime_image_walk_state_snapshot image_walk{};
  objc3_runtime_realized_class_graph_state_snapshot graph{};
  objc3_runtime_realized_class_entry_snapshot imported_entry{};
  objc3_runtime_realized_class_entry_snapshot local_entry{};
  int registration_copy_status = 0;
  int image_walk_status = 0;
  int graph_status = 0;
  int imported_entry_status = 0;
  int local_entry_status = 0;
  std::string imported_module_name;
  std::string imported_translation_unit_identity_key;
  std::string imported_class_owner_identity;
  std::string local_module_name;
  std::string local_translation_unit_identity_key;
  std::string local_class_owner_identity;

  int ImportedClassReceiver() const {
    return MatrixClassReceiver(imported_entry);
  }

  int LocalClassReceiver() const {
    return MatrixClassReceiver(local_entry);
  }
};

struct PostResetRuntimeState {
  objc3_runtime_registration_state_snapshot registration{};
  objc3_runtime_reset_replay_state_snapshot replay{};
  int registration_copy_status = 0;
  int replay_copy_status = 0;
};

struct PostReplayFixtureModuleState {
  objc3_runtime_registration_state_snapshot registration{};
  objc3_runtime_image_walk_state_snapshot image_walk{};
  objc3_runtime_realized_class_graph_state_snapshot graph{};
  objc3_runtime_reset_replay_state_snapshot replay{};
  objc3_runtime_realized_class_entry_snapshot imported_entry{};
  objc3_runtime_realized_class_entry_snapshot local_entry{};
  int registration_copy_status = 0;
  int image_walk_status = 0;
  int graph_status = 0;
  int replay_copy_status = 0;
  int imported_entry_status = 0;
  int local_entry_status = 0;

  int ImportedClassReceiver() const {
    return MatrixClassReceiver(imported_entry);
  }

  int LocalClassReceiver() const {
    return MatrixClassReceiver(local_entry);
  }
};

struct ImportExecutionCaseValues {
  int imported_provider_class_value = 0;
  int imported_provider_protocol_value = 0;
  int local_consumer_class_value = 0;
};

struct StartupMatrixAssertions {
  objc3_runtime_selector_lookup_table_state_snapshot selector_table{};
  objc3_runtime_selector_lookup_entry_snapshot provider_selector{};
  objc3_runtime_selector_lookup_entry_snapshot imported_protocol_selector{};
  objc3_runtime_selector_lookup_entry_snapshot local_selector{};
  objc3_runtime_method_cache_state_snapshot method_cache_state{};
  objc3_runtime_method_cache_entry_snapshot provider_method{};
  objc3_runtime_method_cache_entry_snapshot imported_protocol_method{};
  objc3_runtime_method_cache_entry_snapshot local_method{};
  objc3_runtime_protocol_conformance_query_snapshot protocol_query{};
  int selector_table_status = 0;
  int provider_selector_status = 0;
  int imported_protocol_selector_status = 0;
  int local_selector_status = 0;
  int method_cache_state_status = 0;
  int provider_method_status = 0;
  int imported_protocol_method_status = 0;
  int local_method_status = 0;
  int protocol_query_status = 0;
  std::string method_cache_last_selector;
  std::string method_cache_last_resolved_class_name;
  std::string method_cache_last_resolved_owner_identity;
  std::string provider_method_owner_identity;
  std::string imported_protocol_method_owner_identity;
  std::string local_method_owner_identity;
};

struct PostReplayMatrixAssertions {
  objc3_runtime_selector_lookup_table_state_snapshot selector_table{};
  objc3_runtime_selector_lookup_entry_snapshot provider_selector{};
  objc3_runtime_selector_lookup_entry_snapshot imported_protocol_selector{};
  objc3_runtime_selector_lookup_entry_snapshot local_selector{};
  objc3_runtime_method_cache_state_snapshot method_cache_state{};
  objc3_runtime_method_cache_entry_snapshot provider_method{};
  objc3_runtime_method_cache_entry_snapshot imported_protocol_method{};
  objc3_runtime_method_cache_entry_snapshot local_method{};
  int selector_table_status = 0;
  int provider_selector_status = 0;
  int imported_protocol_selector_status = 0;
  int local_selector_status = 0;
  int method_cache_state_status = 0;
  int provider_method_status = 0;
  int imported_protocol_method_status = 0;
  int local_method_status = 0;
};

struct ProbeResult {
  StartupFixtureModuleState startup_fixture;
  ImportExecutionCaseValues startup_execution;
  StartupMatrixAssertions startup_matrix;
  PostResetRuntimeState post_reset;
  int replay_status = 0;
  PostReplayFixtureModuleState post_replay_fixture;
  ImportExecutionCaseValues post_replay_execution;
  PostReplayMatrixAssertions post_replay_matrix;
};

}  // namespace objc3c::runtime::probe::import_module_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_PROBE_RESULT_H_
