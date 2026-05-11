#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

struct CategoryAttachmentProtocolValues {
  int category_value = 0;
  int class_value = 0;
  int protocol_strict_error = 0;
  int protocol_strict_error_expected = 0;
};

struct RealizedGraphStateObservation {
  objc3_runtime_realized_class_graph_state_snapshot state{};
  std::string class_name;
  std::string class_owner;
  std::string metaclass_owner;
  std::string category_owner;
  std::string category_name;
};

struct RealizedClassEntryObservation {
  objc3_runtime_realized_class_entry_snapshot entry{};
  std::string module;
  std::string identity;
  std::string class_name;
  std::string class_owner;
  std::string metaclass_owner;
  std::string super_class_owner;
  std::string super_metaclass_owner;
  std::string category_owner;
  std::string category_name;
};

struct ProtocolConformanceObservation {
  objc3_runtime_protocol_conformance_query_snapshot query{};
  std::string class_name;
  std::string protocol_name;
  std::string protocol_owner;
  std::string attachment_owner;
};

struct MethodCacheStateObservation {
  objc3_runtime_method_cache_state_snapshot state{};
  std::string selector;
  std::string class_name;
  std::string owner;
};

struct CategoryAttachmentProtocolProbeRun {
  CategoryAttachmentProtocolValues values;
  RealizedGraphStateObservation graph_state;
  RealizedClassEntryObservation widget_entry;
  RealizedClassEntryObservation base_entry;
  ProtocolConformanceObservation worker_query;
  ProtocolConformanceObservation tracer_query;
  ProtocolConformanceObservation base_worker_query;
  MethodCacheStateObservation method_state;
};

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
