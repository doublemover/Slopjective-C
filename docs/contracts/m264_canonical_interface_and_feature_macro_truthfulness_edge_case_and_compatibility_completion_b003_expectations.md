# M264 Canonical Interface And Feature-Macro Truthfulness Edge-Case And Compatibility Completion Expectations (B003)

Contract ID: `objc3c-canonical-interface-and-feature-macro-truthfulness/m264-b003-v1`

Scope: `M264` lane-B completion work that keeps the existing compatibility/strictness semantic packet truthful for separate-compilation and feature-macro claims.

Requirements:

1. The canonical semantic packet remains:
   - `frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics`
2. The packet publishes the current separate-compilation truth surface explicitly:
   - `canonical_interface_truth_model = no-standalone-interface-payload-yet-and-any-future-canonical-interface-must-stay-bounded-to-runnable-and-source-downgraded-claims`
   - `separate_compilation_macro_truth_model = suppressed-feature-macro-claims-remain-unpublished-across-manifest-interface-and-conformance-surfaces-until-executable`
   - `canonical_interface_payload_mode = no-standalone-interface-payload-yet`
3. The packet preserves the exact suppressed macro-claim list in-order:
   - `macro-claim:__OBJC3_STRICTNESS_LEVEL__`
   - `macro-claim:__OBJC3_CONCURRENCY_MODE__`
   - `macro-claim:__OBJC3_CONCURRENCY_STRICT__`
4. Positive probes keep the packet truthful on both live compatibility selections:
   - canonical native compilation
   - legacy compatibility plus migration-assist manifest-only compilation
5. The same packet remains fail-closed and runnable-ready without widening current claims:
   - compatibility and migration-assist remain the only live selection surfaces
   - canonical interface claims remain equivalent-only rather than over-claimed as a standalone emitted textual-interface payload
   - strictness and strict-concurrency macro claims remain suppressed
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
   - `tmp/reports/m264/M264-B003/canonical_interface_and_feature_macro_truthfulness_summary.json`
8. Validation commands:
   - `python scripts/check_m264_b003_canonical_interface_and_feature_macro_truthfulness_edge_case_and_compatibility_completion.py`
   - `python -m pytest tests/tooling/test_check_m264_b003_canonical_interface_and_feature_macro_truthfulness_edge_case_and_compatibility_completion.py -q`
   - `python scripts/run_m264_b003_lane_b_readiness.py`
