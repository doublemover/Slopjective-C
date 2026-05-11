#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

inline void PrintMetaclassGraphRootClassProbeReport(const ProbeRun &run) {
  const RuntimeBootstrapFixture &fixture = run.fixture;
  const MetaclassGraphAssertions &graph = run.graph_assertions;
  const RootClassInvariants &root_class = run.root_class_invariants;

  std::printf("{");
  std::printf("\"root_class_value\":%d,", graph.root_class_value);
  std::printf("\"widget_class_value\":%d,", graph.widget_class_value);
  std::printf("\"widget_known_class_value\":%d,",
              graph.widget_known_class_value);
  std::printf("\"widget_inherited_instance_value\":%d,",
              root_class.widget_inherited_instance_value);
  std::printf("\"widget_own_instance_value\":%d,",
              root_class.widget_own_instance_value);
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      fixture.registration_state);
  std::printf(",\"graph_state\":");
  ::objc3c::runtime::probe::PrintRealizedGraphStateMetaclass(
      fixture.graph_state);
  std::printf(",\"root_entry\":");
  ::objc3c::runtime::probe::PrintRealizedEntryMetaclass(fixture.root_entry);
  std::printf(",\"widget_entry\":");
  ::objc3c::runtime::probe::PrintRealizedEntryMetaclass(fixture.widget_entry);
  std::printf(",\"root_class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMetaclass(
      graph.root_class_state);
  std::printf(",\"widget_class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMetaclass(
      graph.widget_class_state);
  std::printf(",\"widget_known_class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMetaclass(
      graph.widget_known_class_state);
  std::printf(",\"widget_inherited_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMetaclass(
      root_class.widget_inherited_state);
  std::printf(",\"widget_own_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMetaclass(
      root_class.widget_own_state);
  std::printf(",\"root_shared_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryMetaclassMinimal(
      root_class.root_shared_entry);
  std::printf(",\"widget_shared_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryMetaclassMinimal(
      root_class.widget_shared_entry);
  std::printf(",\"widget_inherited_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryMetaclassMinimal(
      root_class.widget_inherited_entry);
  std::printf(",\"widget_own_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryMetaclassMinimal(
      root_class.widget_own_entry);
  std::printf("}\n");
}

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
