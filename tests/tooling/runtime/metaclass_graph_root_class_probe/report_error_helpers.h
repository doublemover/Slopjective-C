#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

inline void PrintRealizedEntryMetaclassGraphRootClass(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  ::objc3c::runtime::probe::PrintIntField("found", snapshot.found);
  ::objc3c::runtime::probe::PrintUint64Field(
      "base_identity", static_cast<unsigned long long>(snapshot.base_identity));
  ::objc3c::runtime::probe::PrintUint64Field(
      "instance_receiver_identity",
      static_cast<unsigned long long>(snapshot.instance_receiver_identity));
  ::objc3c::runtime::probe::PrintUint64Field(
      "class_receiver_identity",
      static_cast<unsigned long long>(snapshot.class_receiver_identity));
  ::objc3c::runtime::probe::PrintUint64Field(
      "registration_order_ordinal",
      static_cast<unsigned long long>(snapshot.registration_order_ordinal));
  ::objc3c::runtime::probe::PrintIntField("is_root_class",
                                          snapshot.is_root_class);
  ::objc3c::runtime::probe::PrintIntField("has_super_node",
                                          snapshot.has_super_node);
  ::objc3c::runtime::probe::PrintIntField("implementation_backed",
                                          snapshot.implementation_backed);
  ::objc3c::runtime::probe::PrintUint64Field(
      "super_base_identity",
      static_cast<unsigned long long>(snapshot.super_base_identity));
  ::objc3c::runtime::probe::PrintStringField("module_name",
                                             snapshot.module_name);
  ::objc3c::runtime::probe::PrintStringField(
      "translation_unit_identity_key",
      snapshot.translation_unit_identity_key);
  ::objc3c::runtime::probe::PrintStringField("class_name",
                                             snapshot.class_name);
  ::objc3c::runtime::probe::PrintStringField(
      "class_owner_identity", snapshot.class_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "metaclass_owner_identity", snapshot.metaclass_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "super_class_owner_identity", snapshot.super_class_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "super_metaclass_owner_identity",
      snapshot.super_metaclass_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "instance_isa_owner_identity", snapshot.instance_isa_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "class_object_isa_owner_identity",
      snapshot.class_object_isa_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "metaclass_object_isa_owner_identity",
      snapshot.metaclass_object_isa_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "root_class_owner_identity", snapshot.root_class_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "root_metaclass_owner_identity", snapshot.root_metaclass_owner_identity,
      false);
  std::printf("}");
}

inline void PrintMetaclassGraphRootClassProbeReport(const ProbeRun &run) {
  const RuntimeBootstrapFixture &fixture = run.fixture;
  const MetaclassGraphAssertions &graph = run.graph_assertions;
  const RootClassInvariants &root_class = run.root_class_invariants;
  const FailClosedDiagnostics &fail_closed = run.fail_closed_diagnostics;

  std::printf("{");
  std::printf("\"root_class_value\":%d,", graph.root_class_value);
  std::printf("\"widget_class_value\":%d,", graph.widget_class_value);
  std::printf("\"widget_known_class_value\":%d,",
              graph.widget_known_class_value);
  std::printf("\"widget_inherited_instance_value\":%d,",
              root_class.widget_inherited_instance_value);
  std::printf("\"widget_own_instance_value\":%d,",
              root_class.widget_own_instance_value);
  std::printf("\"widget_super_instance_value\":%d,",
              root_class.widget_super_instance_value);
  std::printf("\"widget_super_own_selector_status\":%d,",
              root_class.widget_super_own_selector_status);
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      fixture.registration_state);
  std::printf(",\"graph_state\":");
  ::objc3c::runtime::probe::PrintRealizedGraphStateMetaclass(
      fixture.graph_state);
  std::printf(",\"root_entry\":");
  PrintRealizedEntryMetaclassGraphRootClass(fixture.root_entry);
  std::printf(",\"widget_entry\":");
  PrintRealizedEntryMetaclassGraphRootClass(fixture.widget_entry);
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
  std::printf(",\"widget_super_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryMetaclassMinimal(
      root_class.widget_super_entry);
  std::printf(",\"fail_closed_diagnostics\":{");
  ::objc3c::runtime::probe::PrintIntField(
      "root_super_metadata_rejected",
      fail_closed.root_super_metadata_rejected);
  ::objc3c::runtime::probe::PrintStringField(
      "root_super_metadata_reason",
      fail_closed.root_super_metadata_reason.c_str());
  ::objc3c::runtime::probe::PrintIntField(
      "subclass_missing_super_metadata_rejected",
      fail_closed.subclass_missing_super_metadata_rejected);
  ::objc3c::runtime::probe::PrintStringField(
      "subclass_missing_super_metadata_reason",
      fail_closed.subclass_missing_super_metadata_reason.c_str());
  ::objc3c::runtime::probe::PrintIntField(
      "subclass_metaclass_link_mismatch_rejected",
      fail_closed.subclass_metaclass_link_mismatch_rejected);
  ::objc3c::runtime::probe::PrintStringField(
      "subclass_metaclass_link_mismatch_reason",
      fail_closed.subclass_metaclass_link_mismatch_reason.c_str(), false);
  std::printf("}");
  std::printf("}\n");
}

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
