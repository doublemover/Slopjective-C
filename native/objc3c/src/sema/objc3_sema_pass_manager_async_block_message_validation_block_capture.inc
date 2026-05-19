  const auto &block_literal_summary =
      surface.block_literal_capture_semantics_summary;
  record.block_literal_capture_semantics_ready =
      deterministic_block_literal_capture_semantics_handoff &&
      block_literal_summary.block_literal_sites ==
          surface.block_literal_capture_semantics_sites_total &&
      block_literal_summary.block_parameter_entries ==
          surface.block_literal_capture_semantics_parameter_entries_total &&
      block_literal_summary.block_capture_entries ==
          surface.block_literal_capture_semantics_capture_entries_total &&
      block_literal_summary.block_body_statement_entries ==
          surface.block_literal_capture_semantics_body_statement_entries_total &&
      block_literal_summary.block_empty_capture_sites ==
          surface.block_literal_capture_semantics_empty_capture_sites_total &&
      block_literal_summary.block_nondeterministic_capture_sites ==
          surface
              .block_literal_capture_semantics_nondeterministic_capture_sites_total &&
      block_literal_summary.block_non_normalized_sites ==
          surface.block_literal_capture_semantics_non_normalized_sites_total &&
      block_literal_summary.contract_violation_sites ==
          surface.block_literal_capture_semantics_contract_violation_sites_total &&
      block_literal_summary.block_empty_capture_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.block_nondeterministic_capture_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.block_non_normalized_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.contract_violation_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.deterministic;
