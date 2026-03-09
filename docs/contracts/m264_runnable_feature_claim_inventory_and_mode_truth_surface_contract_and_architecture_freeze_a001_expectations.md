# M264 Runnable Feature-Claim Inventory And Mode Truth Surface Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-runnable-feature-claim-inventory/m264-a001-v1`

Scope: `M264` lane-A freeze of the frontend/source-model packet that makes
Objective-C 3 versioning and conformance claims truthful against the currently
implemented runnable native subset.

Requirements:

1. The native frontend emits one canonical packet at
   `frontend.pipeline.semantic_surface.objc_runnable_feature_claim_inventory`.
2. The packet reports:
   - effective language mode `objc3-v1-native-subset`
   - effective language version `3`
   - effective compatibility mode `canonical` or `legacy`
   - whether migration assist is enabled
   - `strictness_selection_supported=false`
   - `strict_concurrency_mode_supported=false`
   - `mode_truth_fail_closed=true`
3. The packet separates three inventories:
   - runnable feature claims
   - source-only feature claims
   - unsupported fail-closed feature claims
4. The runnable inventory includes:
   - `runnable:module-declaration`
   - `runnable:global-let`
   - `runnable:function-bodies`
   - `runnable:extern-prototypes`
   - `runnable:scalar-core`
   - `runnable:control-flow`
   - `runnable:message-send-basic`
5. The source-only inventory includes:
   - `source-only:protocol-declarations`
   - `source-only:interface-declarations`
   - `source-only:implementation-declarations`
   - `source-only:category-declarations`
   - `source-only:property-declarations`
   - `source-only:object-pointer-nullability-generics`
6. The unsupported inventory includes:
   - `unsupported:strictness-selection`
   - `unsupported:strict-concurrency-selection`
   - `unsupported:throws`
   - `unsupported:async-await`
   - `unsupported:actors`
   - `unsupported:blocks`
   - `unsupported:arc`
7. The packet publishes a deterministic replay key and coverage counters derived
   from the live parser snapshot.
8. Explicit anchors must remain present in:
   - `docs/objc3c-native.md`
   - `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
   - `spec/DECISIONS_LOG.md`
   - `native/objc3c/src/token/objc3_token_contract.h`
   - `native/objc3c/src/lex/objc3_lexer.cpp`
   - `native/objc3c/src/parse/objc3_parser.cpp`
9. Validation evidence lands at
   `tmp/reports/m264/M264-A001/runnable_feature_claim_inventory_and_mode_truth_surface_summary.json`.
10. Validation commands:
   - native runnable probes use `artifacts/bin/objc3c-native.exe`
   - manifest-only source-surface probes use `artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
   - `python scripts/check_m264_a001_runnable_feature_claim_inventory_and_mode_truth_surface_contract_and_architecture_freeze.py`
   - `python -m pytest tests/tooling/test_check_m264_a001_runnable_feature_claim_inventory_and_mode_truth_surface_contract_and_architecture_freeze.py -q`
   - `npm run check:objc3c:m264-a001-lane-a-readiness`
