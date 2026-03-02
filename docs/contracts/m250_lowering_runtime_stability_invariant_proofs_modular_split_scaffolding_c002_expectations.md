# Lowering/Runtime Stability and Invariant Proofs Modular Split Scaffolding Expectations (M250-C002)

Contract ID: `objc3c-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding/m250-c002-v1`
Status: Accepted
Scope: `native/objc3c/src/pipeline/*` lowering/runtime stability modular split scaffold continuity.

## Objective

Freeze the C002 modular split/scaffolding boundary so lowering/runtime stability and invariant-proof readiness remain deterministic and fail-closed between typed sema-to-lowering and parse/lowering readiness surfaces.

## Deterministic Invariants

1. `Objc3LoweringRuntimeStabilityInvariantScaffold` remains the canonical modular split scaffold surface:
   - `typed_surface_present`
   - `parse_readiness_surface_present`
   - `lowering_boundary_ready`
   - `runtime_dispatch_contract_consistent`
   - `typed_handoff_key_deterministic`
   - `typed_core_feature_consistent`
   - `parse_ready_for_lowering`
   - `invariant_proofs_ready`
   - `modular_split_ready`
2. `BuildObjc3LoweringRuntimeStabilityInvariantScaffold(...)` remains the only canonical closure-builder for C002 modular split continuity.
3. `RunObjc3FrontendPipeline(...)` wires `BuildObjc3LoweringRuntimeStabilityInvariantScaffold(...)` and stores the resulting scaffold in `Objc3FrontendPipelineResult`.
4. `native/objc3c/src/ARCHITECTURE.md` remains authoritative for the M250 lane-C C002 modular split scaffold anchor.
5. C001 freeze anchors remain mandatory prerequisites:
   - `docs/contracts/m250_lowering_runtime_stability_invariant_proofs_c001_expectations.md`
   - `scripts/check_m250_c001_lowering_runtime_stability_invariant_proofs_contract.py`
   - `tests/tooling/test_check_m250_c001_lowering_runtime_stability_invariant_proofs_contract.py`
   - `spec/planning/compiler/m250/m250_c001_lowering_runtime_stability_invariant_proofs_contract_freeze.md`

## Validation

- `python scripts/check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m250-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m250/M250-C002/lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract_summary.json`
