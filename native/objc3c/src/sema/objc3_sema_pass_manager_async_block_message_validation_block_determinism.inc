  const auto &block_determinism_summary =
      surface.block_determinism_perf_baseline_summary;
  record.block_determinism_perf_baseline_ready =
      deterministic_block_determinism_perf_baseline_handoff &&
      block_determinism_summary.block_literal_sites ==
          surface.block_determinism_perf_baseline_sites_total &&
      block_determinism_summary.baseline_weight_total ==
          surface.block_determinism_perf_baseline_weight_total &&
      block_determinism_summary.parameter_entries_total ==
          surface.block_determinism_perf_baseline_parameter_entries_total &&
      block_determinism_summary.capture_entries_total ==
          surface.block_determinism_perf_baseline_capture_entries_total &&
      block_determinism_summary.body_statement_entries_total ==
          surface.block_determinism_perf_baseline_body_statement_entries_total &&
      block_determinism_summary.deterministic_capture_sites ==
          surface
              .block_determinism_perf_baseline_deterministic_capture_sites_total &&
      block_determinism_summary.heavy_tier_sites ==
          surface.block_determinism_perf_baseline_heavy_tier_sites_total &&
      block_determinism_summary.normalized_profile_sites ==
          surface
              .block_determinism_perf_baseline_normalized_profile_sites_total &&
      block_determinism_summary.contract_violation_sites ==
          surface
              .block_determinism_perf_baseline_contract_violation_sites_total &&
      block_determinism_summary.deterministic_capture_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.heavy_tier_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.normalized_profile_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.contract_violation_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.deterministic;
