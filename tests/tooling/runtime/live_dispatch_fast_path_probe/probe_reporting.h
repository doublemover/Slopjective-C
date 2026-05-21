#pragma once

#include <iostream>
#include <ostream>

#include "fixture_types.h"
#include "support/runtime_snapshot_text.h"

namespace objc3c {
namespace tooling {
namespace live_dispatch_fast_path_probe {

inline void WriteEntry(const char *label,
                       const MethodCacheEntryObservation &observation,
                       std::ostream &out) {
  const auto &snapshot = observation.entry;

  out << label << "_abi_version=" << snapshot.abi_version << "\n";
  out << label << "_found=" << snapshot.found << "\n";
  out << label << "_resolved=" << snapshot.resolved << "\n";
  out << label << "_cache_entry_generation="
      << snapshot.cache_entry_generation << "\n";
  out << label << "_miss_status=" << snapshot.miss_status << "\n";
  out << label << "_fast_path_seeded=" << snapshot.fast_path_seeded << "\n";
  out << label << "_effective_direct_dispatch="
      << snapshot.effective_direct_dispatch << "\n";
  out << label << "_objc_final_declared=" << snapshot.objc_final_declared
      << "\n";
  out << label << "_objc_sealed_declared=" << snapshot.objc_sealed_declared
      << "\n";
  out << label << "_selector=" << observation.selector << "\n";
  out << label << "_fast_path_reason=" << observation.fast_path_reason << "\n";
}

inline void WriteCacheAwareRecord(
    const char *label,
    const CacheAwareDispatchObservation &observation,
    std::ostream &out) {
  const auto &record = observation.record;

  out << label << "_copy_status=" << observation.status << "\n";
  out << label << "_abi_version=" << record.abi_version << "\n";
  out << label << "_descriptor_flags=" << record.descriptor_flags << "\n";
  out << label << "_descriptor_valid=" << record.descriptor_valid << "\n";
  out << label << "_fallback_used=" << record.fallback_used << "\n";
  out << label << "_used_cache=" << record.used_cache << "\n";
  out << label << "_used_fast_path=" << record.used_fast_path << "\n";
  out << label << "_strict_error=" << record.strict_error << "\n";
  out << label << "_status_code=" << record.status_code << "\n";
  out << label << "_invalidation_reason=" << record.invalidation_reason
      << "\n";
  out << label << "_selector_stable_id=" << record.selector_stable_id << "\n";
  out << label << "_normalized_receiver_identity="
      << record.normalized_receiver_identity << "\n";
  out << label << "_cache_entry_generation="
      << record.cache_entry_generation << "\n";
  out << label << "_class_graph_generation="
      << record.class_graph_generation << "\n";
  out << label << "_category_attachment_generation="
      << record.category_attachment_generation << "\n";
  out << label << "_protocol_declaration_generation="
      << record.protocol_declaration_generation << "\n";
  out << label << "_storage_surface_generation="
      << record.storage_surface_generation << "\n";
  out << label << "_method_surface_generation="
      << record.method_surface_generation << "\n";
  out << label << "_method_target_identity="
      << record.method_target_identity << "\n";
  out << label << "_source_line=" << record.source_line << "\n";
  out << label << "_source_column=" << record.source_column << "\n";
  out << label << "_selector=" << observation.selector << "\n";
  out << label << "_source_path=" << observation.source_path << "\n";
  out << label << "_dispatch_path=" << observation.dispatch_path << "\n";
  out << label << "_implementation_kind="
      << observation.implementation_kind << "\n";
  out << label << "_diagnostic_code=" << observation.diagnostic_code << "\n";
}

inline void WriteCounterDeltas(const ProbeRun &run, std::ostream &out) {
  const auto &baseline = run.baseline.state;
  const auto &direct = run.direct.state;
  const auto &mixed_first = run.mixed_first.state;
  const auto &mixed_second = run.mixed_second.state;
  const auto &strict_first = run.strict_error_first.state;
  const auto &strict_second = run.strict_error_second.state;

  out << "direct_delta_cache_entry_count="
      << (direct.cache_entry_count - baseline.cache_entry_count) << "\n";
  out << "direct_delta_cache_hit_count="
      << (direct.cache_hit_count - baseline.cache_hit_count) << "\n";
  out << "direct_delta_fast_path_hit_count="
      << (direct.fast_path_hit_count - baseline.fast_path_hit_count) << "\n";
  out << "direct_delta_live_dispatch_count="
      << (direct.live_dispatch_count - baseline.live_dispatch_count) << "\n";
  out << "mixed_first_delta_cache_hit_count="
      << (mixed_first.cache_hit_count - direct.cache_hit_count) << "\n";
  out << "mixed_first_delta_cache_miss_count="
      << (mixed_first.cache_miss_count - direct.cache_miss_count) << "\n";
  out << "mixed_first_delta_slow_path_lookup_count="
      << (mixed_first.slow_path_lookup_count - direct.slow_path_lookup_count)
      << "\n";
  out << "mixed_first_delta_fast_path_hit_count="
      << (mixed_first.fast_path_hit_count - direct.fast_path_hit_count)
      << "\n";
  out << "mixed_first_delta_live_dispatch_count="
      << (mixed_first.live_dispatch_count - direct.live_dispatch_count) << "\n";
  out << "mixed_second_delta_cache_hit_count="
      << (mixed_second.cache_hit_count - mixed_first.cache_hit_count) << "\n";
  out << "mixed_second_delta_fast_path_hit_count="
      << (mixed_second.fast_path_hit_count - mixed_first.fast_path_hit_count)
      << "\n";
  out << "mixed_second_delta_live_dispatch_count="
      << (mixed_second.live_dispatch_count - mixed_first.live_dispatch_count)
      << "\n";
  out << "strict_error_first_delta_cache_entry_count="
      << (strict_first.cache_entry_count - mixed_second.cache_entry_count)
      << "\n";
  out << "strict_error_first_delta_cache_miss_count="
      << (strict_first.cache_miss_count - mixed_second.cache_miss_count)
      << "\n";
  out << "strict_error_first_delta_slow_path_lookup_count="
      << (strict_first.slow_path_lookup_count -
          mixed_second.slow_path_lookup_count)
      << "\n";
  out << "strict_error_first_delta_strict_dispatch_error_count="
      << (strict_first.strict_dispatch_error_count -
          mixed_second.strict_dispatch_error_count)
      << "\n";
  out << "strict_error_second_delta_cache_hit_count="
      << (strict_second.cache_hit_count - strict_first.cache_hit_count) << "\n";
  out << "strict_error_second_delta_strict_dispatch_error_count="
      << (strict_second.strict_dispatch_error_count -
          strict_first.strict_dispatch_error_count)
      << "\n";
}

inline void WriteStatusAndReturnValues(const ProbeRun &run, std::ostream &out) {
  out << "baseline_status=" << run.baseline.status << "\n";
  out << "dynamic_entry_status=" << run.dynamic_entry.status << "\n";
  out << "explicit_entry_status=" << run.explicit_entry.status << "\n";
  out << "implicit_value=" << run.implicit_value << "\n";
  out << "explicit_value=" << run.explicit_value << "\n";
  out << "direct_status=" << run.direct.status << "\n";
  out << "mixed_first=" << run.mixed_first_value << "\n";
  out << "mixed_first_status=" << run.mixed_first.status << "\n";
  out << "mixed_first_dispatch_state_status="
      << run.mixed_first_dispatch.status << "\n";
  out << "mixed_second=" << run.mixed_second_value << "\n";
  out << "mixed_second_status=" << run.mixed_second.status << "\n";
  out << "mixed_second_dispatch_state_status="
      << run.mixed_second_dispatch.status << "\n";
  out << "strict_error_expected=" << run.strict_error_expected << "\n";
  out << "strict_error_first=" << run.strict_error_first_value << "\n";
  out << "strict_error_first_status=" << run.strict_error_first.status << "\n";
  out << "strict_error_first_dispatch_state_status="
      << run.strict_error_first_dispatch.status << "\n";
  out << "strict_error_second=" << run.strict_error_second_value << "\n";
  out << "strict_error_second_status=" << run.strict_error_second.status
      << "\n";
  out << "strict_error_second_dispatch_state_status="
      << run.strict_error_second_dispatch.status << "\n";
  out << "strict_error_entry_status=" << run.strict_error_entry.status << "\n";
  out << "cache_aware_prepare_status="
      << run.cache_aware_prepare_status << "\n";
  out << "cache_aware_value=" << run.cache_aware_value << "\n";
  out << "cache_aware_stale_value=" << run.cache_aware_stale_value << "\n";
  out << "cache_aware_malformed_status="
      << run.cache_aware_malformed_status << "\n";
  out << "cache_aware_missing_validation_status="
      << run.cache_aware_missing_validation_status << "\n";
}

inline void WriteMethodCacheSnapshots(const ProbeRun &run, std::ostream &out) {
  ::objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState(
      out, "baseline", run.baseline.state, run.baseline.last_selector,
      run.baseline.last_fast_path_reason);
  ::objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState(
      out, "direct", run.direct.state, run.direct.last_selector,
      run.direct.last_fast_path_reason);
  ::objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState(
      out, "mixed_first_state", run.mixed_first.state,
      run.mixed_first.last_selector, run.mixed_first.last_fast_path_reason);
  ::objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState(
      out, "mixed_second_state", run.mixed_second.state,
      run.mixed_second.last_selector, run.mixed_second.last_fast_path_reason);
  ::objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState(
      out, "strict_error_first_state", run.strict_error_first.state,
      run.strict_error_first.last_selector,
      run.strict_error_first.last_fast_path_reason);
  ::objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState(
      out, "strict_error_second_state", run.strict_error_second.state,
      run.strict_error_second.last_selector,
      run.strict_error_second.last_fast_path_reason);
}

inline void WriteDispatchSnapshots(const ProbeRun &run, std::ostream &out) {
  ::objc3c::runtime::probe::WriteLabeledDispatchState(
      out, "mixed_first_dispatch_state", run.mixed_first_dispatch.state,
      run.mixed_first_dispatch.last_selector,
      run.mixed_first_dispatch.last_fast_path_reason,
      run.mixed_first_dispatch.last_path,
      run.mixed_first_dispatch.last_implementation_kind,
      run.mixed_first_dispatch.last_resolved_class_name);
  ::objc3c::runtime::probe::WriteLabeledDispatchState(
      out, "mixed_second_dispatch_state", run.mixed_second_dispatch.state,
      run.mixed_second_dispatch.last_selector,
      run.mixed_second_dispatch.last_fast_path_reason,
      run.mixed_second_dispatch.last_path,
      run.mixed_second_dispatch.last_implementation_kind,
      run.mixed_second_dispatch.last_resolved_class_name);
  ::objc3c::runtime::probe::WriteLabeledDispatchState(
      out, "strict_error_first_dispatch_state",
      run.strict_error_first_dispatch.state,
      run.strict_error_first_dispatch.last_selector,
      run.strict_error_first_dispatch.last_fast_path_reason,
      run.strict_error_first_dispatch.last_path,
      run.strict_error_first_dispatch.last_implementation_kind,
      run.strict_error_first_dispatch.last_resolved_class_name);
  ::objc3c::runtime::probe::WriteLabeledDispatchState(
      out, "strict_error_second_dispatch_state",
      run.strict_error_second_dispatch.state,
      run.strict_error_second_dispatch.last_selector,
      run.strict_error_second_dispatch.last_fast_path_reason,
      run.strict_error_second_dispatch.last_path,
      run.strict_error_second_dispatch.last_implementation_kind,
      run.strict_error_second_dispatch.last_resolved_class_name);
}

inline void WriteProbeReport(const ProbeRun &run, std::ostream &out) {
  WriteStatusAndReturnValues(run, out);
  WriteCounterDeltas(run, out);
  WriteEntry("dynamic_entry", run.dynamic_entry, out);
  WriteEntry("explicit_entry", run.explicit_entry, out);
  WriteEntry("strict_error_entry", run.strict_error_entry, out);
  WriteCacheAwareRecord(
      "cache_aware_dispatch", run.cache_aware_dispatch, out);
  WriteCacheAwareRecord(
      "cache_aware_stale_dispatch", run.cache_aware_stale_dispatch, out);
  WriteCacheAwareRecord(
      "cache_aware_malformed_dispatch",
      run.cache_aware_malformed_dispatch, out);
  WriteCacheAwareRecord(
      "cache_aware_missing_validation_dispatch",
      run.cache_aware_missing_validation_dispatch, out);
  WriteMethodCacheSnapshots(run, out);
  WriteDispatchSnapshots(run, out);
}

inline void WriteProbeReportToStdout(const ProbeRun &run) {
  WriteProbeReport(run, std::cout);
}

} // namespace live_dispatch_fast_path_probe
} // namespace tooling
} // namespace objc3c
