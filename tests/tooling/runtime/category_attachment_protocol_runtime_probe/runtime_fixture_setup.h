#pragma once

#include "category_protocol_fixture_definitions.h"
#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline void StabilizeRealizedGraphStateObservation(
    RealizedGraphStateObservation &observation) {
  ::objc3c::runtime::probe::StabilizeGraphState(
      observation.state, observation.class_name, observation.class_owner,
      observation.metaclass_owner, observation.category_owner,
      observation.category_name);
}

inline void CaptureRealizedGraphState(
    RealizedGraphStateObservation &observation) {
  observation = RealizedGraphStateObservation{};
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &observation.state);
  StabilizeRealizedGraphStateObservation(observation);
}

inline void StabilizeRealizedClassEntryObservation(
    RealizedClassEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeRealizedEntry(
      observation.entry, observation.module, observation.identity,
      observation.class_name, observation.class_owner,
      observation.metaclass_owner, observation.super_class_owner,
      observation.super_metaclass_owner, observation.category_owner,
      observation.category_name);
}

inline void CaptureRealizedClassEntry(
    const char *class_name, RealizedClassEntryObservation &observation) {
  observation = RealizedClassEntryObservation{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      class_name, &observation.entry);
  StabilizeRealizedClassEntryObservation(observation);
}

inline void StabilizeProtocolConformanceObservation(
    ProtocolConformanceObservation &observation) {
  ::objc3c::runtime::probe::StabilizeConformanceQuery(
      observation.query, observation.class_name, observation.protocol_name,
      observation.protocol_owner, observation.attachment_owner,
      &observation.matched_class_name, &observation.matched_class_owner);
}

inline void CaptureProtocolConformanceQuery(
    const char *class_name, const char *protocol_name,
    ProtocolConformanceObservation &observation) {
  observation = ProtocolConformanceObservation{};
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      class_name, protocol_name, &observation.query);
  StabilizeProtocolConformanceObservation(observation);
}

inline void CaptureCategoryAttachmentProtocolRuntimeSnapshots(
    CategoryAttachmentProtocolProbeRun &run) {
  CaptureRealizedGraphState(run.graph_state);
  CaptureRealizedClassEntry(kWidgetClassName, run.widget_entry);
  CaptureRealizedClassEntry(kBaseClassName, run.base_entry);
  CaptureProtocolConformanceQuery(kWidgetClassName, kWorkerProtocolName,
                                  run.worker_query);
  CaptureProtocolConformanceQuery(kWidgetClassName, kTracerProtocolName,
                                  run.tracer_query);
  CaptureProtocolConformanceQuery(kBaseClassName, kWorkerProtocolName,
                                  run.base_worker_query);
  CaptureProtocolConformanceQuery(kDerivedClassName, kWorkerProtocolName,
                                  run.derived_worker_query);
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
