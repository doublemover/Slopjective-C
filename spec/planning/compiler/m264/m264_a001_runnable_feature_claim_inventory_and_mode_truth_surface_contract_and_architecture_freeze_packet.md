# M264-A001 Runnable Feature-Claim Inventory And Mode Truth Surface Contract And Architecture Freeze Packet

Packet: `M264-A001`  
Milestone: `M264`  
Lane: `A`  
Contract ID: `objc3c-runnable-feature-claim-inventory/m264-a001-v1`

Summary:

Freeze one canonical frontend packet that makes current Objective-C 3 language
mode, compatibility-mode, and feature-claim reporting truthful against the
actually runnable native subset.

Dependencies:

- `M259-E002`
- `M263-E002`

Code anchors:

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`

Spec anchors:

- `docs/objc3c-native.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
- `spec/DECISIONS_LOG.md`

Required outputs:

- `frontend.pipeline.semantic_surface.objc_runnable_feature_claim_inventory`
- `tmp/reports/m264/M264-A001/runnable_feature_claim_inventory_and_mode_truth_surface_summary.json`

Validation:

- runnable happy-path probes use `artifacts/bin/objc3c-native.exe`
- manifest-only source-surface probes use `artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
- `check:objc3c:m264-a001-runnable-feature-claim-inventory-and-mode-truth-surface`
- `test:tooling:m264-a001-runnable-feature-claim-inventory-and-mode-truth-surface`
- `check:objc3c:m264-a001-lane-a-readiness`

Truth constraints:

- runnable claims remain limited to the currently executable native subset
- source-only claims remain explicitly non-runnable
- unsupported claims remain fail-closed
- strictness and strict concurrency remain unclaimed until later milestones land
