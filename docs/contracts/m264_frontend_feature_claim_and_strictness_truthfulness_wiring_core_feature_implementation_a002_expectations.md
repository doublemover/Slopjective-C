# M264 Frontend Feature-Claim And Strictness Truthfulness Wiring Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-feature-claim-strictness-truth-surface/m264-a002-v1`

Scope: `M264` lane-A implementation of the truthful frontend selection and feature-claim surface that stays aligned with the currently runnable native Objective-C 3 subset.

Requirements:

1. The frontend manifest publishes one top-level truth surface under `frontend` with:
   - `default_compatibility_mode`
   - `language_version_selection_supported`
   - `compatibility_selection_supported`
   - `migration_assist_selection_supported`
   - `strictness_selection_supported`
   - `strict_concurrency_selection_supported`
   - `feature_macro_surface_supported`
   - `feature_claim_truth_surface_contract_id`
2. The semantic surface publishes one canonical packet at:
   - `frontend.pipeline.semantic_surface.objc_feature_claim_and_strictness_truth_surface`
3. The packet reports truthfully that:
   - language-version selection is supported
   - compatibility selection is supported
   - migration assist selection is supported
   - strictness selection is not yet supported
   - strict concurrency selection is not yet supported
   - feature-macro claim publication is not yet supported
4. The packet links back to `M264-A001` via `runnable_feature_claim_inventory_contract_id`.
5. The packet keeps one fail-closed truth model and one explicit driver-surface model.
6. The packet publishes deterministic arrays for:
   - supported selection surfaces
   - unsupported selection surfaces
   - suppressed macro claims
7. Dynamic validation proves:
   - native runnable truth on `artifacts/bin/objc3c-native.exe`
   - legacy/source-only manifest truth on `artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
8. Explicit anchors remain present in:
   - `docs/objc3c-native.md`
   - `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
   - `spec/DECISIONS_LOG.md`
   - `native/objc3c/src/token/objc3_token_contract.h`
   - `native/objc3c/src/lex/objc3_lexer.cpp`
   - `native/objc3c/src/parse/objc3_parser.cpp`
9. Validation evidence lands at:
   - `tmp/reports/m264/M264-A002/frontend_feature_claim_and_strictness_truthfulness_wiring_summary.json`
10. Validation commands:
   - `python scripts/check_m264_a002_frontend_feature_claim_and_strictness_truthfulness_wiring_core_feature_implementation.py`
   - `python -m pytest tests/tooling/test_check_m264_a002_frontend_feature_claim_and_strictness_truthfulness_wiring_core_feature_implementation.py -q`
   - `python scripts/run_m264_a002_lane_a_readiness.py`
