#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_MATRIX_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_MATRIX_ASSERTIONS_H_

#include "import_module_execution_matrix_probe/probe_result.h"

namespace objc3c::runtime::probe::import_module_execution_matrix {

inline void StabilizeStartupMatrixAssertions(StartupMatrixAssertions &state) {
  state.method_cache_last_selector =
      CopyRuntimeCString(state.method_cache_state.last_selector);
  state.method_cache_last_resolved_class_name =
      CopyRuntimeCString(state.method_cache_state.last_resolved_class_name);
  state.method_cache_last_resolved_owner_identity =
      CopyRuntimeCString(state.method_cache_state.last_resolved_owner_identity);
  state.provider_method_owner_identity =
      CopyRuntimeCString(state.provider_method.resolved_owner_identity);
  state.imported_protocol_method_owner_identity = CopyRuntimeCString(
      state.imported_protocol_method.resolved_owner_identity);
  state.local_method_owner_identity =
      CopyRuntimeCString(state.local_method.resolved_owner_identity);
}

inline StartupMatrixAssertions CaptureStartupMatrixAssertions(
    const StartupFixtureModuleState &fixture) {
  StartupMatrixAssertions state;
  state.selector_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(
          &state.selector_table);
  state.provider_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          kProviderClassValueSelector, &state.provider_selector);
  state.imported_protocol_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          kImportedProtocolValueSelector, &state.imported_protocol_selector);
  state.local_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          kLocalClassValueSelector, &state.local_selector);
  state.method_cache_state_status =
      objc3_runtime_copy_method_cache_state_for_testing(
          &state.method_cache_state);
  state.provider_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          fixture.ImportedClassReceiver(), kProviderClassValueSelector,
          &state.provider_method);
  state.imported_protocol_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          fixture.ImportedClassReceiver(), kImportedProtocolValueSelector,
          &state.imported_protocol_method);
  state.local_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          fixture.LocalClassReceiver(), kLocalClassValueSelector,
          &state.local_method);
  state.protocol_query_status =
      objc3_runtime_copy_protocol_conformance_query_for_testing(
          kImportedProviderClassName, kImportedWorkerProtocolName,
          &state.protocol_query);
  return state;
}

inline PostReplayMatrixAssertions CapturePostReplayMatrixAssertions(
    const PostReplayFixtureModuleState &fixture) {
  PostReplayMatrixAssertions state;
  state.selector_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(
          &state.selector_table);
  state.provider_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          kProviderClassValueSelector, &state.provider_selector);
  state.imported_protocol_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          kImportedProtocolValueSelector, &state.imported_protocol_selector);
  state.local_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          kLocalClassValueSelector, &state.local_selector);
  state.method_cache_state_status =
      objc3_runtime_copy_method_cache_state_for_testing(
          &state.method_cache_state);
  state.provider_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          fixture.ImportedClassReceiver(), kProviderClassValueSelector,
          &state.provider_method);
  state.imported_protocol_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          fixture.ImportedClassReceiver(), kImportedProtocolValueSelector,
          &state.imported_protocol_method);
  state.local_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          fixture.LocalClassReceiver(), kLocalClassValueSelector,
          &state.local_method);
  return state;
}

}  // namespace objc3c::runtime::probe::import_module_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_MATRIX_ASSERTIONS_H_
