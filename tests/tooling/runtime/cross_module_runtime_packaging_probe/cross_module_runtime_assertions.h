#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_CROSS_MODULE_RUNTIME_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_CROSS_MODULE_RUNTIME_ASSERTIONS_H_

#include "cross_module_runtime_packaging_probe/probe_result.h"

namespace objc3c::runtime::probe::cross_module_runtime_packaging {

inline CrossModuleRuntimeDispatchValues ExecuteCrossModuleRuntimeDispatches(
    int imported_class_receiver, int local_class_receiver) {
  CrossModuleRuntimeDispatchValues values;
  values.imported_provider_class_value = objc3_runtime_dispatch_i32(
      imported_class_receiver, kProviderClassValueSelector, 0, 0, 0, 0);
  values.imported_provider_protocol_value = objc3_runtime_dispatch_i32(
      imported_class_receiver, kImportedProtocolValueSelector, 0, 0, 0, 0);
  values.local_consumer_class_value = objc3_runtime_dispatch_i32(
      local_class_receiver, kLocalClassValueSelector, 0, 0, 0, 0);
  return values;
}

inline CrossModuleRuntimeDispatchValues ExecuteStartupRuntimeDispatches(
    const StartupPackageModuleState &state) {
  return ExecuteCrossModuleRuntimeDispatches(state.ImportedClassReceiver(),
                                            state.LocalClassReceiver());
}

inline CrossModuleRuntimeDispatchValues ExecutePostReplayRuntimeDispatches(
    const PostReplayPackageModuleState &state) {
  return ExecuteCrossModuleRuntimeDispatches(state.ImportedClassReceiver(),
                                            state.LocalClassReceiver());
}

inline CrossModuleRuntimeAssertions CaptureStartupRuntimeAssertions(
    const StartupPackageModuleState &state) {
  CrossModuleRuntimeAssertions assertions;
  assertions.dispatch_values = ExecuteStartupRuntimeDispatches(state);
  assertions.imported_worker_query_status =
      objc3_runtime_copy_protocol_conformance_query_for_testing(
          kImportedProviderClassName, kImportedWorkerProtocolName,
          &assertions.imported_worker_query);
  return assertions;
}

}  // namespace objc3c::runtime::probe::cross_module_runtime_packaging

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_CROSS_MODULE_RUNTIME_ASSERTIONS_H_
