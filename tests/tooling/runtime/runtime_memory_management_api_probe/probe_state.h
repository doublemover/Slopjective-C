#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_PROBE_STATE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_PROBE_STATE_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::runtime_memory_management_api {

struct MemoryManagementFixture {
  int parent = 0;
  int child = 0;
};

struct RealizedClassGraphCapture {
  objc3_runtime_realized_class_graph_state_snapshot snapshot{};
  std::string class_name_storage;
};

struct MemoryManagementGraphCaptures {
  RealizedClassGraphCapture after_alloc;
  RealizedClassGraphCapture after_helper_release;
  RealizedClassGraphCapture after_clear;
  RealizedClassGraphCapture after_parent_release;
};

struct MemoryManagementOperationResults {
  int parent = 0;
  int child = 0;
  int strong_set_result = 0;
  int weak_set_result = 0;
  int retain_result = 0;
  int autorelease_result = 0;
  int release_after_helper_result = 0;
  int release_local_result = 0;
  int strong_before_clear = 0;
  int weak_before_clear = 0;
  int clear_strong_result = 0;
  int strong_after_clear = 0;
  int weak_after_clear = 0;
  int parent_release_result = 0;
};

struct MemoryManagementProbeRun {
  objc3_runtime_registration_state_snapshot registration_state{};
  MemoryManagementOperationResults operations;
  MemoryManagementGraphCaptures graphs;
};

}  // namespace objc3c::runtime::probe::runtime_memory_management_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_PROBE_STATE_H_
