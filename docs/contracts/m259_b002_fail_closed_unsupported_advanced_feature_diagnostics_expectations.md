# M259 B002 Fail-Closed Unsupported Advanced Feature Diagnostics Expectations

Contract ID: `objc3c-runnable-core-unsupported-advanced-feature-diagnostics/m259-b002-v1`

Issue: `#7211`

## Objective

Turn the runnable-core compatibility guard into a live fail-closed diagnostic
boundary for accepted advanced source surfaces that are still outside the native
Objective-C 3 runnable subset.

## Required implementation

1. Add the issue-local contract/checker/test/readiness assets:
   - `docs/contracts/m259_b002_fail_closed_unsupported_advanced_feature_diagnostics_expectations.md`
   - `spec/planning/compiler/m259/m259_b002_fail_closed_unsupported_advanced_feature_diagnostics_packet.md`
   - `scripts/check_m259_b002_fail_closed_unsupported_advanced_feature_diagnostics_core_feature_implementation.py`
   - `tests/tooling/test_check_m259_b002_fail_closed_unsupported_advanced_feature_diagnostics_core_feature_implementation.py`
   - `scripts/run_m259_b002_lane_b_readiness.py`
2. Keep the live semantic packet explicit in:
   - `native/objc3c/src/sema/objc3_sema_contract.h`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_types.h`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
3. Publish explicit docs/spec/package/script anchors in:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
   - `package.json`
4. Prove the live runnable/unsupported split with deterministic probes:
   - a positive runnable-core probe compiles successfully and keeps
     `frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics`
     ready with zero live unsupported rejection sites
   - negative source probes for `throws`, `@autoreleasepool`, and ARC ownership
     qualifiers fail closed with deterministic `O3S221` diagnostics
   - negative probes do not publish a manifest and do not reach lowering/runtime
     handoff
5. Keep the current truthful boundary explicit:
   - the integrated `M259-A002` runnable sample remains the canonical positive
     proof floor
   - `M259-B001` remains the guard/freeze dependency
   - block literals remain documented as unsupported without over-claiming a
     dedicated live B002 proof path while the parser surface is still gated
     earlier than this issue’s canonical negative coverage
6. The contract must explicitly hand off to `M259-C001`.

## Canonical models

- Guard model:
  `runnable-core-crossing-into-unsupported-advanced-surfaces-fails-before-lowering-runtime-handoff`
- Evidence model:
  `a002-runnable-proof-plus-b001-guard-plus-live-o3s221-negative-source-probes`
- Failure model:
  `fail-closed-on-runnable-core-unsupported-advanced-feature-diagnostic-drift`

## Evidence

- `tmp/reports/m259/M259-B002/fail_closed_unsupported_advanced_feature_diagnostics_summary.json`
