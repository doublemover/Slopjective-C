          handoff.inline_asm_intrinsic_governance_summary.deterministic &&
          handoff.inline_asm_intrinsic_governance_summary
                  .inline_asm_intrinsic_sites ==
              inline_asm_intrinsic_governance_summary.inline_asm_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
              inline_asm_intrinsic_governance_summary.inline_asm_sites &&
          handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
              inline_asm_intrinsic_governance_summary.intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites ==
              inline_asm_intrinsic_governance_summary.governed_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary
                  .privileged_intrinsic_sites ==
              inline_asm_intrinsic_governance_summary
                  .privileged_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary.normalized_sites ==
              inline_asm_intrinsic_governance_summary.normalized_sites &&
          handoff.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
              inline_asm_intrinsic_governance_summary.gate_blocked_sites &&
          handoff.inline_asm_intrinsic_governance_summary
                  .contract_violation_sites ==
              inline_asm_intrinsic_governance_summary.contract_violation_sites &&
          handoff.inline_asm_intrinsic_governance_summary.inline_asm_sites <=
              handoff.inline_asm_intrinsic_governance_summary
                  .inline_asm_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites <=
              handoff.inline_asm_intrinsic_governance_summary
                  .inline_asm_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites <=
              handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary
                  .privileged_intrinsic_sites <=
              handoff.inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary.normalized_sites <=
              handoff.inline_asm_intrinsic_governance_summary
                  .inline_asm_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary.gate_blocked_sites <=
              handoff.inline_asm_intrinsic_governance_summary
                  .inline_asm_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary
                  .contract_violation_sites <=
              handoff.inline_asm_intrinsic_governance_summary
                  .inline_asm_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
              handoff.throws_propagation_summary
                  .cache_invalidation_candidate_sites &&
          handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
              handoff.unsafe_pointer_extension_summary.unsafe_operation_sites &&
          handoff.inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites <=
              handoff.throws_propagation_summary.normalized_sites &&
          handoff.inline_asm_intrinsic_governance_summary
                  .privileged_intrinsic_sites <=
              handoff.unsafe_pointer_extension_summary.normalized_sites &&
          handoff.inline_asm_intrinsic_governance_summary.normalized_sites >=
              handoff.inline_asm_intrinsic_governance_summary.inline_asm_sites &&
          handoff.inline_asm_intrinsic_governance_summary.normalized_sites -
                  handoff.inline_asm_intrinsic_governance_summary
                      .inline_asm_sites ==
              handoff.inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
              handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites -
                  handoff.inline_asm_intrinsic_governance_summary
                      .governed_intrinsic_sites &&
          handoff.inline_asm_intrinsic_governance_summary.normalized_sites +
                  handoff.inline_asm_intrinsic_governance_summary
                      .gate_blocked_sites ==
              handoff.inline_asm_intrinsic_governance_summary
                  .inline_asm_intrinsic_sites &&
