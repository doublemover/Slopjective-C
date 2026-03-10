# M264-B002 Fail-Closed Unsupported Feature Claim Enforcement Core Feature Implementation Packet

Packet: `M264-B002`  
Milestone: `M264`  
Lane: `B`  
Issue: `#7236`  
Contract ID: `objc3c-fail-closed-unsupported-feature-claim-enforcement/m264-b002-v1`

Summary:

Implement the live semantic rejection path for accepted source surfaces that are
still not runnable in the native Objective-C 3 subset, while keeping the
existing `M264-B001` semantic legality packet as the single source of truth.

Dependencies:

- `M264-A002`
- `M264-B001`

Code anchors:

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

Spec anchors:

- `docs/objc3c-native.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
- `spec/DECISIONS_LOG.md`

Required outputs:

- `frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics`
- `tmp/reports/m264/M264-B002/fail_closed_unsupported_feature_claim_enforcement_summary.json`

Validation:

- `check:objc3c:m264-b002-fail-closed-unsupported-feature-claim-enforcement`
- `test:tooling:m264-b002-fail-closed-unsupported-feature-claim-enforcement`
- `check:objc3c:m264-b002-lane-b-readiness`

Truth constraints:

- accepted unsupported source surfaces must fail before lowering/runtime handoff
- the semantic legality packet remains the only contract authority for this lane
- positive runnable/source-only probes publish zero live unsupported rejection
  counts
- `throws`, `@autoreleasepool`, and executable ARC ownership qualifiers produce
  deterministic `O3S221` diagnostics
- block literals remain tracked as unsupported without overstating a live B002
  proof path while the parser gate still fires earlier
