#pragma once

#include <iostream>
#include <ostream>

#include "fixture_types.h"

#include "support/runtime_snapshot_text.h"

namespace objc3c {
namespace tooling {
namespace runtime_fast_path_contract_probe {

inline void WriteStatusAndReturnValues(const ProbeRun &run, std::ostream &out) {
  out << "baseline_status=" << run.baseline.status << "\n";
  out << "implicit_value=" << run.implicit_value << "\n";
  out << "explicit_value=" << run.explicit_value << "\n";
  out << "direct_status=" << run.direct.status << "\n";
  out << "mixed_first=" << run.mixed_first_value << "\n";
  out << "mixed_first_status=" << run.mixed_first.status << "\n";
  out << "mixed_second=" << run.mixed_second_value << "\n";
  out << "mixed_second_status=" << run.mixed_second.status << "\n";
  out << "strict_error_expected=" << run.strict_error_expected << "\n";
  out << "strict_error_first=" << run.strict_error_first_value << "\n";
  out << "strict_error_first_status=" << run.strict_error_first.status << "\n";
  out << "strict_error_second=" << run.strict_error_second_value << "\n";
  out << "strict_error_second_status=" << run.strict_error_second.status
      << "\n";
  out << "dynamic_entry_status=" << run.dynamic_entry.status << "\n";
  out << "dynamic_entry_found=" << run.dynamic_entry.entry.found << "\n";
  out << "dynamic_entry_resolved=" << run.dynamic_entry.entry.resolved << "\n";
  out << "strict_error_entry_status=" << run.strict_error_entry.status << "\n";
  out << "strict_error_entry_found=" << run.strict_error_entry.entry.found
      << "\n";
  out << "strict_error_entry_resolved=" << run.strict_error_entry.entry.resolved
      << "\n";
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
  out << "direct_delta_cache_miss_count="
      << (direct.cache_miss_count - baseline.cache_miss_count) << "\n";
  out << "direct_delta_live_dispatch_count="
      << (direct.live_dispatch_count - baseline.live_dispatch_count) << "\n";
  out << "mixed_first_delta_cache_entry_count="
      << (mixed_first.cache_entry_count - direct.cache_entry_count) << "\n";
  out << "mixed_first_delta_cache_miss_count="
      << (mixed_first.cache_miss_count - direct.cache_miss_count) << "\n";
  out << "mixed_first_delta_slow_path_lookup_count="
      << (mixed_first.slow_path_lookup_count - direct.slow_path_lookup_count)
      << "\n";
  out << "mixed_first_delta_live_dispatch_count="
      << (mixed_first.live_dispatch_count - direct.live_dispatch_count) << "\n";
  out << "mixed_second_delta_cache_hit_count="
      << (mixed_second.cache_hit_count - mixed_first.cache_hit_count) << "\n";
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

inline void WriteMethodCacheSnapshots(const ProbeRun &run, std::ostream &out) {
  ::objc3c::runtime::probe::WriteLabeledMethodCacheState(
      out, "baseline", run.baseline.state, run.baseline.last_selector);
  ::objc3c::runtime::probe::WriteLabeledMethodCacheState(
      out, "direct", run.direct.state, run.direct.last_selector);
  ::objc3c::runtime::probe::WriteLabeledMethodCacheState(
      out, "mixed_first_state", run.mixed_first.state,
      run.mixed_first.last_selector);
  ::objc3c::runtime::probe::WriteLabeledMethodCacheState(
      out, "mixed_second_state", run.mixed_second.state,
      run.mixed_second.last_selector);
  ::objc3c::runtime::probe::WriteLabeledMethodCacheState(
      out, "strict_error_first_state", run.strict_error_first.state,
      run.strict_error_first.last_selector);
  ::objc3c::runtime::probe::WriteLabeledMethodCacheState(
      out, "strict_error_second_state", run.strict_error_second.state,
      run.strict_error_second.last_selector);
}

inline void WriteProbeReport(const ProbeRun &run, std::ostream &out) {
  WriteStatusAndReturnValues(run, out);
  WriteCounterDeltas(run, out);
  WriteMethodCacheSnapshots(run, out);
}

inline void WriteProbeReportToStdout(const ProbeRun &run) {
  WriteProbeReport(run, std::cout);
}

} // namespace runtime_fast_path_contract_probe
} // namespace tooling
} // namespace objc3c
