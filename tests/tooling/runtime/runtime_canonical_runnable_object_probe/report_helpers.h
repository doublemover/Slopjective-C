#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdio>
#include <string>

namespace objc3c {
namespace runtime {
namespace probe {
namespace runtime_canonical_runnable_object {

struct MethodCacheStateStorage {
  std::string selector;
  std::string class_name;
  std::string owner;
};

struct MethodCacheEntryStorage {
  std::string selector;
  std::string class_name;
  std::string owner;
};

struct ConformanceQueryStorage {
  std::string class_name;
  std::string protocol;
  std::string protocol_owner;
  std::string attachment_owner;
  std::string failure_reason;
};

struct ProbeReportStorage {
  std::string graph_class;
  std::string graph_class_owner;
  std::string graph_metaclass_owner;
  std::string graph_category_owner;
  std::string graph_category_name;
  std::string widget_module;
  std::string widget_identity;
  std::string widget_class;
  std::string widget_class_owner;
  std::string widget_metaclass_owner;
  std::string widget_super_class_owner;
  std::string widget_super_metaclass_owner;
  std::string widget_category_owner;
  std::string widget_category_name;
  ConformanceQueryStorage worker_query;
  ConformanceQueryStorage tracer_query;
  MethodCacheStateStorage method_state;
  MethodCacheStateStorage inherited_state;
  MethodCacheStateStorage traced_state;
  MethodCacheStateStorage class_state;
  MethodCacheStateStorage ignored_state;
  MethodCacheStateStorage ignored_cached_state;
  MethodCacheEntryStorage alloc_entry;
  MethodCacheEntryStorage init_entry;
  MethodCacheEntryStorage new_entry;
  MethodCacheEntryStorage traced_entry;
  MethodCacheEntryStorage inherited_entry;
  MethodCacheEntryStorage class_entry;
  MethodCacheEntryStorage ignored_entry;
  std::string selector_table_last;
  std::string traced_selector_canonical;
  std::string inherited_selector_canonical;
  std::string class_selector_canonical;
  std::string ignored_selector_canonical;
};

inline void StabilizeConformanceQuerySnapshot(
    objc3_runtime_protocol_conformance_query_snapshot &query,
    ConformanceQueryStorage &storage) {
  ::objc3c::runtime::probe::StabilizeConformanceQuery(
      query, storage.class_name, storage.protocol, storage.protocol_owner,
      storage.attachment_owner, nullptr, nullptr, &storage.failure_reason);
}

inline void StabilizeMethodCacheStateSnapshot(
    objc3_runtime_method_cache_state_snapshot &state,
    MethodCacheStateStorage &storage) {
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      state, storage.selector, storage.class_name, storage.owner);
}

inline void StabilizeMethodCacheEntrySnapshot(
    objc3_runtime_method_cache_entry_snapshot &entry,
    MethodCacheEntryStorage &storage) {
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      entry, storage.selector, storage.class_name, storage.owner);
}

inline void StabilizeProbeReport(ProbeRun &run, ProbeReportStorage &storage) {
  RuntimeFixture &fixture = run.fixture;
  RunnableInvocationAssertions &runnable = run.runnable;
  ObjectClassAssertions &object_class = run.object_class;

  ::objc3c::runtime::probe::StabilizeGraphState(
      fixture.graph_state, storage.graph_class, storage.graph_class_owner,
      storage.graph_metaclass_owner, storage.graph_category_owner,
      storage.graph_category_name);
  ::objc3c::runtime::probe::StabilizeRealizedEntry(
      fixture.widget_entry, storage.widget_module, storage.widget_identity,
      storage.widget_class, storage.widget_class_owner,
      storage.widget_metaclass_owner, storage.widget_super_class_owner,
      storage.widget_super_metaclass_owner, storage.widget_category_owner,
      storage.widget_category_name);
  StabilizeConformanceQuerySnapshot(object_class.worker_query,
                                    storage.worker_query);
  StabilizeConformanceQuerySnapshot(object_class.tracer_query,
                                    storage.tracer_query);
  StabilizeMethodCacheStateSnapshot(object_class.method_state,
                                    storage.method_state);
  StabilizeMethodCacheStateSnapshot(runnable.inherited_state,
                                    storage.inherited_state);
  StabilizeMethodCacheStateSnapshot(runnable.traced_state,
                                    storage.traced_state);
  StabilizeMethodCacheStateSnapshot(runnable.class_state, storage.class_state);
  StabilizeMethodCacheStateSnapshot(runnable.ignored_state,
                                    storage.ignored_state);
  StabilizeMethodCacheStateSnapshot(runnable.ignored_cached_state,
                                    storage.ignored_cached_state);
  StabilizeMethodCacheEntrySnapshot(object_class.alloc_entry,
                                    storage.alloc_entry);
  StabilizeMethodCacheEntrySnapshot(object_class.init_entry,
                                    storage.init_entry);
  StabilizeMethodCacheEntrySnapshot(object_class.new_entry, storage.new_entry);
  StabilizeMethodCacheEntrySnapshot(object_class.traced_entry,
                                    storage.traced_entry);
  StabilizeMethodCacheEntrySnapshot(object_class.inherited_entry,
                                    storage.inherited_entry);
  StabilizeMethodCacheEntrySnapshot(object_class.class_entry,
                                    storage.class_entry);
  StabilizeMethodCacheEntrySnapshot(object_class.ignored_entry,
                                    storage.ignored_entry);
  ::objc3c::runtime::probe::StabilizeSelectorTableState(
      object_class.selector_table_state, storage.selector_table_last);
  ::objc3c::runtime::probe::StabilizeSelectorEntry(
      object_class.traced_selector_entry, storage.traced_selector_canonical);
  ::objc3c::runtime::probe::StabilizeSelectorEntry(
      object_class.inherited_selector_entry,
      storage.inherited_selector_canonical);
  ::objc3c::runtime::probe::StabilizeSelectorEntry(
      object_class.class_selector_entry, storage.class_selector_canonical);
  ::objc3c::runtime::probe::StabilizeSelectorEntry(
      object_class.ignored_selector_entry, storage.ignored_selector_canonical);
}

inline unsigned long long SelectorStableId(
    const objc3_runtime_selector_handle *selector) {
  return static_cast<unsigned long long>(
      selector != nullptr ? selector->stable_id : 0);
}

inline void PrintSelectorHandles(const RuntimeFixture &fixture) {
  std::printf("\"selector_handles\":{");
  std::printf("\"alloc\":%llu,", SelectorStableId(fixture.alloc_selector));
  std::printf("\"init\":%llu,", SelectorStableId(fixture.init_selector));
  std::printf("\"new\":%llu,", SelectorStableId(fixture.new_selector));
  std::printf("\"tracedValue\":%llu,",
              SelectorStableId(fixture.traced_selector));
  std::printf("\"inheritedValue\":%llu,",
              SelectorStableId(fixture.inherited_selector));
  std::printf("\"classValue\":%llu},",
              SelectorStableId(fixture.class_selector));
}

inline void PrintRuntimeCanonicalRunnableObjectReport(ProbeRun &run) {
  ProbeReportStorage storage;
  StabilizeProbeReport(run, storage);

  const RuntimeFixture &fixture = run.fixture;
  const RunnableInvocationAssertions &runnable = run.runnable;
  const ObjectClassAssertions &object_class = run.object_class;

  std::printf("{");
  std::printf("\"alloc_value\":%d,", runnable.alloc_value);
  std::printf("\"init_value\":%d,", runnable.init_value);
  std::printf("\"new_value\":%d,", runnable.new_value);
  std::printf("\"traced_value\":%d,", runnable.traced_value);
  std::printf("\"inherited_value\":%d,", runnable.inherited_value);
  std::printf("\"class_value\":%d,", runnable.class_value);
  std::printf("\"ignored_value\":%d,", runnable.ignored_value);
  std::printf("\"ignored_cached_value\":%d,", runnable.ignored_cached_value);
  std::printf("\"ignored_expected\":%d,", runnable.ignored_expected);
  std::printf("\"widget_instance_receiver\":%d,",
              fixture.widget_instance_receiver);
  std::printf("\"widget_class_receiver\":%d,", fixture.widget_class_receiver);
  PrintSelectorHandles(fixture);
  std::printf("\"graph_state\":");
  ::objc3c::runtime::probe::PrintGraphStateProtocolCategory(
      fixture.graph_state);
  std::printf(",\"widget_entry\":");
  ::objc3c::runtime::probe::PrintRealizedEntryProtocolCategory(
      fixture.widget_entry);
  std::printf(",\"worker_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryProtocolCategory(
      object_class.worker_query);
  std::printf(",\"tracer_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryProtocolCategory(
      object_class.tracer_query);
  std::printf(",\"method_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(
      object_class.method_state);
  std::printf(",\"inherited_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(
      runnable.inherited_state);
  std::printf(",\"traced_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(runnable.traced_state);
  std::printf(",\"class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(runnable.class_state);
  std::printf(",\"ignored_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(runnable.ignored_state);
  std::printf(",\"ignored_cached_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(
      runnable.ignored_cached_state);
  std::printf(",\"alloc_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryRuntimeCanonical(
      object_class.alloc_entry);
  std::printf(",\"init_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryRuntimeCanonical(
      object_class.init_entry);
  std::printf(",\"new_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryRuntimeCanonical(
      object_class.new_entry);
  std::printf(",\"traced_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryRuntimeCanonical(
      object_class.traced_entry);
  std::printf(",\"inherited_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryRuntimeCanonical(
      object_class.inherited_entry);
  std::printf(",\"class_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryRuntimeCanonical(
      object_class.class_entry);
  std::printf(",\"ignored_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryRuntimeCanonical(
      object_class.ignored_entry);
  std::printf(",\"selector_table_state\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateRuntimeCanonical(
      object_class.selector_table_state);
  std::printf(",\"traced_selector_entry\":");
  ::objc3c::runtime::probe::PrintSelectorEntryBasic(
      object_class.traced_selector_entry);
  std::printf(",\"inherited_selector_entry\":");
  ::objc3c::runtime::probe::PrintSelectorEntryBasic(
      object_class.inherited_selector_entry);
  std::printf(",\"class_selector_entry\":");
  ::objc3c::runtime::probe::PrintSelectorEntryBasic(
      object_class.class_selector_entry);
  std::printf(",\"ignored_selector_entry\":");
  ::objc3c::runtime::probe::PrintSelectorEntryBasic(
      object_class.ignored_selector_entry);
  std::printf("}\n");
}

} // namespace runtime_canonical_runnable_object
} // namespace probe
} // namespace runtime
} // namespace objc3c
