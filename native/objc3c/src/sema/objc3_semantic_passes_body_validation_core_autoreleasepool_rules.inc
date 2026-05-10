         handoff.autoreleasepool_scope_summary.deterministic &&
         handoff.autoreleasepool_scope_summary.scope_sites ==
             autoreleasepool_scope_summary.scope_sites &&
         handoff.autoreleasepool_scope_summary.scope_symbolized_sites ==
             autoreleasepool_scope_summary.scope_symbolized_sites &&
         handoff.autoreleasepool_scope_summary.contract_violation_sites ==
             autoreleasepool_scope_summary.contract_violation_sites &&
         handoff.autoreleasepool_scope_summary.max_scope_depth ==
             autoreleasepool_scope_summary.max_scope_depth &&
         handoff.autoreleasepool_scope_summary.scope_symbolized_sites <=
             handoff.autoreleasepool_scope_summary.scope_sites &&
         handoff.autoreleasepool_scope_summary.contract_violation_sites <=
             handoff.autoreleasepool_scope_summary.scope_sites &&
         (handoff.autoreleasepool_scope_summary.scope_sites > 0u ||
          handoff.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
         handoff.autoreleasepool_scope_summary.max_scope_depth <=
             static_cast<unsigned>(handoff.autoreleasepool_scope_summary.scope_sites);
