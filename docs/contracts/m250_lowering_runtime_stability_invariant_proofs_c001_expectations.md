# M250 Lowering/Runtime Stability and Invariant Proofs Contract Freeze Expectations (C001)

Contract ID: `objc3c-lowering-runtime-stability-invariant-proofs-freeze/m250-c001-v1`
Status: Accepted
Scope: lowering/runtime boundary normalization, typed sema-to-lowering readiness gates, and replay-proof IR stability signals.

## Objective

Freeze lowering/runtime stability boundaries so GA hardening can expand without regressing runtime dispatch boundary determinism, typed handoff fail-closed behavior, or global-proof invalidation safety.

## Required Invariants

1. Lowering contract normalization and replay boundary remain canonical:
   - `native/objc3c/src/lower/objc3_lowering_contract.h` keeps runtime dispatch defaults (`kObjc3RuntimeDispatchDefaultArgs`, `kObjc3RuntimeDispatchMaxArgs`, `kObjc3RuntimeDispatchSymbol`, `kObjc3SelectorGlobalOrdering`).
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp` continues to enforce `TryNormalizeObjc3LoweringContract(...)` and `TryBuildObjc3LoweringIRBoundary(...)`.
   - `Objc3LoweringIRBoundaryReplayKey(...)` remains runtime-dispatch/symbol-ordering stable.
2. Typed sema-to-lowering surface remains fail-closed for runtime dispatch invariants:
   - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h` keeps `surface.runtime_dispatch_contract_consistent`.
   - Typed handoff readiness still requires `surface.typed_handoff_key_deterministic`.
   - Failure modes remain explicit for runtime dispatch inconsistency and typed handoff nondeterminism.
3. Parse/lowering readiness remains chained to typed sema runtime stability gates:
   - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h` projects `typed_handoff_key_deterministic`, `typed_sema_core_feature_consistent`, and `typed_sema_core_feature_key`.
   - Readiness continues fail-closed on typed-handoff and typed core-feature expansion drift.
4. IR emission keeps replay-proof runtime dispatch and proof-invalidation anchors:
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp` still emits `Objc3LoweringIRBoundaryReplayKey(...)` banner metadata.
   - Runtime dispatch slot bounds remain validated at emission.
   - Runtime dispatch proof marker `runtime_dispatch_call_emitted_ = true;` remains explicit.
   - Global proof invalidation marker `ctx.global_proofs_invalidated = true;` remains explicit.
5. Semantics and artifact docs stay aligned to runtime stability gates:
   - `docs/objc3c-native/src/30-semantics.md` preserves deterministic dispatch ABI marshalling invariants and parity-gate anchor `result.parity_surface.deterministic_dispatch_abi_marshalling_handoff`.
   - `docs/objc3c-native/src/50-artifacts.md` preserves M206 canonical optimization packet/source-anchor references including `runtime_dispatch_call_emitted_ = true;`, `ctx.global_proofs_invalidated = true;`, and `Objc3LoweringIRBoundaryReplayKey(...)`.

## Validation

- `python scripts/check_m250_c001_lowering_runtime_stability_invariant_proofs_contract.py`
- `python -m pytest tests/tooling/test_check_m250_c001_lowering_runtime_stability_invariant_proofs_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m250/M250-C001/lowering_runtime_stability_invariant_proofs_contract_summary.json`
