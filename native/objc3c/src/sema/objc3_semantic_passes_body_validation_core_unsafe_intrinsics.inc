          handoff.unsafe_pointer_extension_summary.deterministic &&
          handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites ==
              unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
          handoff.unsafe_pointer_extension_summary.unsafe_keyword_sites ==
              unsafe_pointer_extension_summary.unsafe_keyword_sites &&
          handoff.unsafe_pointer_extension_summary.pointer_arithmetic_sites ==
              unsafe_pointer_extension_summary.pointer_arithmetic_sites &&
          handoff.unsafe_pointer_extension_summary.raw_pointer_type_sites ==
              unsafe_pointer_extension_summary.raw_pointer_type_sites &&
          handoff.unsafe_pointer_extension_summary.unsafe_operation_sites ==
              unsafe_pointer_extension_summary.unsafe_operation_sites &&
          handoff.unsafe_pointer_extension_summary.normalized_sites ==
              unsafe_pointer_extension_summary.normalized_sites &&
          handoff.unsafe_pointer_extension_summary.gate_blocked_sites ==
              unsafe_pointer_extension_summary.gate_blocked_sites &&
          handoff.unsafe_pointer_extension_summary.contract_violation_sites ==
              unsafe_pointer_extension_summary.contract_violation_sites &&
          handoff.unsafe_pointer_extension_summary.unsafe_keyword_sites <=
              handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
          handoff.unsafe_pointer_extension_summary.pointer_arithmetic_sites <=
              handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
          handoff.unsafe_pointer_extension_summary.raw_pointer_type_sites <=
              handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
          handoff.unsafe_pointer_extension_summary.unsafe_operation_sites <=
              handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
          handoff.unsafe_pointer_extension_summary.normalized_sites <=
              handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
          handoff.unsafe_pointer_extension_summary.gate_blocked_sites <=
              handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
          handoff.unsafe_pointer_extension_summary.contract_violation_sites <=
              handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
          handoff.unsafe_pointer_extension_summary.normalized_sites +
                  handoff.unsafe_pointer_extension_summary.gate_blocked_sites ==
              handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
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
