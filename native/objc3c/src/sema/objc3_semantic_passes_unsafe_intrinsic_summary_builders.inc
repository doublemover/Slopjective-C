static Objc3UnsafePointerExtensionSummary
BuildUnsafePointerExtensionSummaryFromTypeAnnotationAndWeakUnownedSummaries(
    const Objc3TypeAnnotationSurfaceSummary &type_annotation_summary,
    const Objc3WeakUnownedSemanticsSummary &weak_unowned_summary) {
  Objc3UnsafePointerExtensionSummary summary;
  summary.raw_pointer_type_sites = type_annotation_summary.pointer_declarator_sites;
  summary.pointer_arithmetic_sites = type_annotation_summary.pointer_declarator_sites;
  summary.unsafe_pointer_extension_sites = summary.pointer_arithmetic_sites;

  std::size_t unsafe_keyword_candidates = 0;
  if (weak_unowned_summary.unowned_reference_sites >= weak_unowned_summary.unowned_safe_reference_sites) {
    unsafe_keyword_candidates =
        weak_unowned_summary.unowned_reference_sites -
        weak_unowned_summary.unowned_safe_reference_sites;
  } else {
    summary.deterministic = false;
  }

  summary.unsafe_keyword_sites =
      std::min(unsafe_keyword_candidates, summary.unsafe_pointer_extension_sites);
  summary.unsafe_operation_sites =
      std::min(summary.pointer_arithmetic_sites, summary.unsafe_keyword_sites);
  summary.normalized_sites = summary.unsafe_operation_sites;
  summary.gate_blocked_sites =
      summary.unsafe_pointer_extension_sites - summary.normalized_sites;
  summary.contract_violation_sites = summary.gate_blocked_sites;
  summary.deterministic =
      summary.deterministic &&
      type_annotation_summary.deterministic &&
      weak_unowned_summary.deterministic &&
      summary.unsafe_keyword_sites <= summary.unsafe_pointer_extension_sites &&
      summary.pointer_arithmetic_sites <= summary.unsafe_pointer_extension_sites &&
      summary.raw_pointer_type_sites <= summary.unsafe_pointer_extension_sites &&
      summary.unsafe_operation_sites <= summary.unsafe_pointer_extension_sites &&
      summary.normalized_sites <= summary.unsafe_pointer_extension_sites &&
      summary.gate_blocked_sites <= summary.unsafe_pointer_extension_sites &&
      summary.contract_violation_sites <= summary.unsafe_pointer_extension_sites &&
      summary.normalized_sites + summary.gate_blocked_sites ==
          summary.unsafe_pointer_extension_sites;
  return summary;
}

static Objc3InlineAsmIntrinsicGovernanceSummary
BuildInlineAsmIntrinsicGovernanceSummaryFromUnsafePointerAndThrowsSummaries(
    const Objc3UnsafePointerExtensionSummary &unsafe_pointer_extension_summary,
    const Objc3ThrowsPropagationSummary &throws_propagation_summary) {
  Objc3InlineAsmIntrinsicGovernanceSummary summary;

  summary.inline_asm_sites = throws_propagation_summary.cache_invalidation_candidate_sites;
  summary.intrinsic_sites = unsafe_pointer_extension_summary.unsafe_operation_sites;

  if (summary.inline_asm_sites >
      std::numeric_limits<std::size_t>::max() - summary.intrinsic_sites) {
    summary.inline_asm_intrinsic_sites = std::numeric_limits<std::size_t>::max();
    summary.contract_violation_sites += 1u;
  } else {
    summary.inline_asm_intrinsic_sites =
        summary.inline_asm_sites + summary.intrinsic_sites;
  }

  summary.governed_intrinsic_sites =
      std::min(summary.intrinsic_sites, throws_propagation_summary.normalized_sites);
  summary.privileged_intrinsic_sites =
      std::min(summary.governed_intrinsic_sites,
               unsafe_pointer_extension_summary.normalized_sites);

  if (summary.inline_asm_sites >
      std::numeric_limits<std::size_t>::max() - summary.governed_intrinsic_sites) {
    summary.normalized_sites = summary.inline_asm_intrinsic_sites;
    summary.contract_violation_sites += 1u;
  } else {
    summary.normalized_sites =
        summary.inline_asm_sites + summary.governed_intrinsic_sites;
  }

  if (summary.normalized_sites <= summary.inline_asm_intrinsic_sites) {
    summary.gate_blocked_sites =
        summary.inline_asm_intrinsic_sites - summary.normalized_sites;
  } else {
    summary.gate_blocked_sites = 0;
    summary.contract_violation_sites += 1u;
  }

  if (summary.inline_asm_sites > summary.inline_asm_intrinsic_sites ||
      summary.intrinsic_sites > summary.inline_asm_intrinsic_sites ||
      summary.governed_intrinsic_sites > summary.intrinsic_sites ||
      summary.privileged_intrinsic_sites > summary.governed_intrinsic_sites ||
      summary.normalized_sites > summary.inline_asm_intrinsic_sites ||
      summary.gate_blocked_sites > summary.inline_asm_intrinsic_sites ||
      summary.normalized_sites + summary.gate_blocked_sites !=
          summary.inline_asm_intrinsic_sites) {
    summary.contract_violation_sites += 1u;
  }

  if (summary.contract_violation_sites > summary.inline_asm_intrinsic_sites) {
    summary.contract_violation_sites = summary.inline_asm_intrinsic_sites;
  }

  summary.deterministic =
      unsafe_pointer_extension_summary.deterministic &&
      throws_propagation_summary.deterministic &&
      summary.inline_asm_sites ==
          throws_propagation_summary.cache_invalidation_candidate_sites &&
      summary.intrinsic_sites ==
          unsafe_pointer_extension_summary.unsafe_operation_sites &&
      summary.inline_asm_sites <= summary.inline_asm_intrinsic_sites &&
      summary.intrinsic_sites <= summary.inline_asm_intrinsic_sites &&
      summary.governed_intrinsic_sites <= summary.intrinsic_sites &&
      summary.governed_intrinsic_sites <=
          throws_propagation_summary.normalized_sites &&
      summary.privileged_intrinsic_sites <= summary.governed_intrinsic_sites &&
      summary.privileged_intrinsic_sites <=
          unsafe_pointer_extension_summary.normalized_sites &&
      summary.normalized_sites <= summary.inline_asm_intrinsic_sites &&
      summary.gate_blocked_sites <= summary.inline_asm_intrinsic_sites &&
      summary.contract_violation_sites <= summary.inline_asm_intrinsic_sites &&
      summary.normalized_sites >= summary.inline_asm_sites &&
      summary.normalized_sites - summary.inline_asm_sites ==
          summary.governed_intrinsic_sites &&
      summary.gate_blocked_sites ==
          summary.intrinsic_sites - summary.governed_intrinsic_sites &&
      summary.normalized_sites + summary.gate_blocked_sites ==
          summary.inline_asm_intrinsic_sites &&
      summary.contract_violation_sites == 0u;
  return summary;
}
