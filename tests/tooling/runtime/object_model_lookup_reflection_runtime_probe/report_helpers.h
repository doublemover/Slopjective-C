#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_REPORT_HELPERS_H_

#include "probe_result.h"
#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime {

inline void PrintAggregateLookupState(
    const AggregateQueryObservation &observation) {
  const objc3_runtime_object_model_query_state_snapshot &aggregate =
      observation.snapshot;

  std::printf("{");
  std::printf("\"realized_class_count\":%llu,",
              static_cast<unsigned long long>(aggregate.realized_class_count));
  std::printf("\"reflectable_property_count\":%llu,",
              static_cast<unsigned long long>(
                  aggregate.reflectable_property_count));
  std::printf("\"attached_category_count\":%llu,",
              static_cast<unsigned long long>(
                  aggregate.attached_category_count));
  std::printf("\"protocol_conformance_edge_count\":%llu,",
              static_cast<unsigned long long>(
                  aggregate.protocol_conformance_edge_count));
  std::printf("\"method_cache_entry_count\":%llu,",
              static_cast<unsigned long long>(
                  aggregate.method_cache_entry_count));
  std::printf("\"last_class_query_found\":%d,",
              aggregate.last_class_query_found);
  std::printf("\"last_property_query_found\":%d,",
              aggregate.last_property_query_found);
  std::printf("\"last_property_query_inherited\":%d,",
              aggregate.last_property_query_inherited);
  std::printf("\"last_protocol_query_class_found\":%d,",
              aggregate.last_protocol_query_class_found);
  std::printf("\"last_protocol_query_protocol_found\":%d,",
              aggregate.last_protocol_query_protocol_found);
  std::printf("\"last_protocol_query_conforms\":%d,",
              aggregate.last_protocol_query_conforms);
  std::printf("\"last_queried_class_name\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_queried_class_name);
  std::printf(",\"last_resolved_class_name\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_resolved_class_name);
  std::printf(",\"last_resolved_class_owner_identity\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_resolved_class_owner_identity);
  std::printf(",\"last_queried_property_name\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_queried_property_name);
  std::printf(",\"last_resolved_property_class_name\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_resolved_property_class_name);
  std::printf(",\"last_resolved_property_owner_identity\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_resolved_property_owner_identity);
  std::printf(",\"last_queried_protocol_class_name\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_queried_protocol_class_name);
  std::printf(",\"last_queried_protocol_name\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_queried_protocol_name);
  std::printf(",\"last_matched_protocol_owner_identity\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_matched_protocol_owner_identity);
  std::printf(",\"last_matched_attachment_owner_identity\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      aggregate.last_matched_attachment_owner_identity);
  std::printf("}");
}

inline void PrintProbeReport(const ProbeResult &result) {
  std::printf("{");
  std::printf("\"widget_found\":%d,",
              result.fixture.widget_class.snapshot.found);
  std::printf("\"traced_value\":%d,", result.dispatch.traced_value);
  std::printf("\"count_value\":%d,", result.dispatch.count_value);
  std::printf("\"count_property_found\":%d,",
              result.reflection.count_property.snapshot.found);
  std::printf("\"tracer_conforms\":%d,",
              result.reflection.tracer_query.snapshot.conforms);
  std::printf("\"aggregate\":");
  PrintAggregateLookupState(result.reflection.aggregate);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_REPORT_HELPERS_H_
