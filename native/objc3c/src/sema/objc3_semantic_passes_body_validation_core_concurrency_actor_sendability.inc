          handoff.actor_isolation_sendability_summary.deterministic &&
          handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites ==
              actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary
                  .actor_isolation_decl_sites ==
              actor_isolation_sendability_summary.actor_isolation_decl_sites &&
          handoff.actor_isolation_sendability_summary.actor_hop_sites ==
              actor_isolation_sendability_summary.actor_hop_sites &&
          handoff.actor_isolation_sendability_summary
                  .sendable_annotation_sites ==
              actor_isolation_sendability_summary.sendable_annotation_sites &&
          handoff.actor_isolation_sendability_summary
                  .non_sendable_crossing_sites ==
              actor_isolation_sendability_summary.non_sendable_crossing_sites &&
          handoff.actor_isolation_sendability_summary
                  .isolation_boundary_sites ==
              actor_isolation_sendability_summary.isolation_boundary_sites &&
          handoff.actor_isolation_sendability_summary.normalized_sites ==
              actor_isolation_sendability_summary.normalized_sites &&
          handoff.actor_isolation_sendability_summary.gate_blocked_sites ==
              actor_isolation_sendability_summary.gate_blocked_sites &&
          handoff.actor_isolation_sendability_summary
                  .contract_violation_sites ==
              actor_isolation_sendability_summary.contract_violation_sites &&
          handoff.actor_isolation_sendability_summary
                  .actor_isolation_decl_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.actor_hop_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary
                  .sendable_annotation_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary
                  .non_sendable_crossing_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary
                  .isolation_boundary_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.normalized_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.gate_blocked_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.gate_blocked_sites <=
              handoff.actor_isolation_sendability_summary
                  .non_sendable_crossing_sites &&
          handoff.actor_isolation_sendability_summary
                  .contract_violation_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.normalized_sites +
                  handoff.actor_isolation_sendability_summary.gate_blocked_sites ==
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
