# M228 Ownership-Aware Lowering Behavior Performance and Quality Guardrails Expectations (B011)

Contract ID: `objc3c-ownership-aware-lowering-behavior-performance-quality-guardrails/m228-b011-v1`
Status: Accepted
Scope: ownership-aware lowering performance/quality guardrails on top of B010 conformance-corpus expansion.

## Objective

Execute issue `#5205` by extending lane-B ownership-aware lowering closure with
explicit performance/quality guardrail accounting and fail-closed readiness so
direct LLVM IR emission cannot proceed on performance/quality drift.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-B readiness.

## Dependency Scope

- Dependencies: `M228-B010`
- M228-B010 remains a mandatory prerequisite for B011 performance/quality
  guardrails:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_conformance_corpus_expansion_b010_expectations.md`
  - `scripts/check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py`
  - `spec/planning/compiler/m228/m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_packet.md`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` carries B011 performance and
   quality guardrail fields:
   - `performance_quality_guardrails_consistent`
   - `performance_quality_guardrails_ready`
   - `performance_quality_guardrails_key`
   - `parse_lowering_performance_quality_guardrails_consistent`
   - `parse_lowering_performance_quality_guardrails_case_count`
   - `parse_lowering_performance_quality_guardrails_passed_case_count`
   - `parse_lowering_performance_quality_guardrails_failed_case_count`
   - `parse_lowering_performance_quality_guardrails_key`
2. `BuildObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsKey(...)`
   remains deterministic and keyed by B010 conformance-corpus continuity plus
   parse-lowering performance/quality accounting continuity.
3. `BuildObjc3OwnershipAwareLoweringBehaviorScaffold(...)` computes
   performance/quality fail-closed from:
   - B010 conformance-corpus readiness/key continuity
   - parse-lowering performance/quality consistency
   - parse-lowering performance/quality case accounting consistency
   - parse-artifact replay-key determinism
4. `IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(...)`
   fails closed when performance/quality consistency/readiness or
   performance-quality-key continuity drifts.
5. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` enforces explicit
   fail-closed lane-B performance/quality gating with deterministic diagnostic
   code `O3L328`.
6. IR metadata projection includes explicit ownership-aware performance/quality
   guardrail readiness and key continuity:
   - `Objc3IRFrontendMetadata::ownership_aware_lowering_performance_quality_guardrails_ready`
   - `Objc3IRFrontendMetadata::ownership_aware_lowering_performance_quality_guardrails_key`
   - emitted IR comments `; ownership_aware_lowering_performance_quality_guardrails = ...`
     and `; ownership_aware_lowering_performance_quality_guardrails_ready = ...`

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m228-b011-ownership-aware-lowering-behavior-performance-quality-guardrails-contract`
  - `test:tooling:m228-b011-ownership-aware-lowering-behavior-performance-quality-guardrails-contract`
  - `check:objc3c:m228-b011-lane-b-readiness`
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m228-b010-lane-b-readiness`
  - `check:objc3c:m228-b011-lane-b-readiness`

## Architecture and Spec Anchors

Shared-file deltas required for full lane-B readiness.

- `native/objc3c/src/ARCHITECTURE.md` includes M228 lane-B B011
  performance/quality guardrails anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes M228 lane-B B011
  fail-closed performance/quality wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B011
  performance/quality metadata anchors.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Validation

- `python scripts/check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py`
- `python scripts/check_m228_b011_ownership_aware_lowering_behavior_performance_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b011_ownership_aware_lowering_behavior_performance_quality_guardrails_contract.py -q`
- `npm run check:objc3c:m228-b011-lane-b-readiness`

## Evidence Path

- `tmp/reports/m228/M228-B011/ownership_aware_lowering_behavior_performance_quality_guardrails_contract_summary.json`
- `tmp/reports/m228/M228-B011/closeout_validation_report.md`
