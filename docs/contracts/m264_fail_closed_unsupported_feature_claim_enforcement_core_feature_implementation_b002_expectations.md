# M264 Fail-Closed Unsupported Feature-Claim Enforcement Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-fail-closed-unsupported-feature-claim-enforcement/m264-b002-v1`

Scope: `M264` lane-B implementation of the live semantic rejection path for
accepted source surfaces that still are not runnable in the native Objective-C 3
subset.

Requirements:

1. The canonical semantic packet remains:
   - `frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics`
2. Positive probes keep that packet truthful with:
   - `live_unsupported_feature_family_count = 0`
   - `live_unsupported_feature_site_count = 0`
   - `live_unsupported_feature_diagnostic_count = 0`
   - `throws_source_rejection_site_count = 0`
   - `blocks_source_rejection_site_count = 0`
   - `arc_source_rejection_site_count = 0`
   - `live_unsupported_feature_source_rejection_landed = true`
   - `ready_for_lowering_and_runtime = true`
   - `ready = true`
3. Accepted unsupported source surfaces fail closed before lowering/runtime
   handoff with deterministic `O3S221` diagnostics:
   - `throws`
   - `@autoreleasepool`
   - ARC ownership qualifiers on executable function/method signatures
4. Negative probes do not over-claim later-lane success:
   - diagnostics artifacts are written
   - manifest publication does not occur
   - lowering/runtime handoff does not proceed
5. Block literals remain documented as unsupported without over-claiming a live
   B002 proof path while the current parser surface is still gated earlier than
   the semantic rejection path.
6. Explicit anchors remain present in:
   - `native/objc3c/src/sema/objc3_sema_contract.h`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_types.h`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
   - `docs/objc3c-native.md`
   - `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
   - `spec/DECISIONS_LOG.md`
   - `package.json`
7. Validation evidence lands at:
   - `tmp/reports/m264/M264-B002/fail_closed_unsupported_feature_claim_enforcement_summary.json`
8. Validation commands:
   - `python scripts/check_m264_b002_fail_closed_unsupported_feature_claim_enforcement_core_feature_implementation.py`
   - `python -m pytest tests/tooling/test_check_m264_b002_fail_closed_unsupported_feature_claim_enforcement_core_feature_implementation.py -q`
   - `python scripts/run_m264_b002_lane_b_readiness.py`
