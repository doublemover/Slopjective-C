# M262 Runnable ARC Runtime Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-runnable-arc-runtime-gate/m262-e001-v1`

Issue: `#7206`

## Objective

Freeze one fail-closed lane-E gate proving the supported runnable ARC slice is
backed by the live native compiler, lowering, and runtime path rather than by
parser-only or metadata-only claims.

## Required implementation

1. Add a canonical expectations document for the runnable ARC gate.
2. Add this packet, a deterministic checker, tooling tests, and a direct
   lane-E readiness runner:
   - `scripts/check_m262_e001_runnable_arc_runtime_gate_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m262_e001_runnable_arc_runtime_gate_contract_and_architecture_freeze.py`
   - `scripts/run_m262_e001_lane_e_readiness.py`
3. Add `M262-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Keep the gate fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m262/M262-A002/arc_mode_handling_summary.json`
   - `tmp/reports/m262/M262-B003/arc_interaction_semantics_summary.json`
   - `tmp/reports/m262/M262-C004/arc_block_autorelease_return_lowering_summary.json`
   - `tmp/reports/m262/M262-D003/arc_debug_instrumentation_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops
   reporting successful coverage, or drops the frozen contract ids.
6. The emitted IR must publish the gate boundary and named metadata:
   - `; runnable_arc_runtime_gate = ...`
   - `!objc3.objc_runnable_arc_runtime_gate`
7. `package.json` must wire:
   - `check:objc3c:m262-e001-runnable-arc-runtime-gate`
   - `test:tooling:m262-e001-runnable-arc-runtime-gate`
   - `check:objc3c:m262-e001-lane-e-readiness`
8. The gate must explicitly hand off to `M262-E002`.

## Canonical models

- Evidence model:
  `a002-b003-c004-d003-summary-chain`
- Active gate model:
  `runnable-arc-gate-consumes-arc-mode-semantics-lowering-and-runtime-proofs-rather-than-parser-only-or-metadata-only-claims`
- Non-goal model:
  `no-runnable-arc-closeout-matrix-no-public-runtime-abi-widening-no-cross-module-arc-claims-before-m262-e002`
- Failure model:
  `fail-closed-on-runnable-arc-runtime-evidence-drift`

## Non-goals

- No new ARC source-surface expansion.
- No new ARC semantic or lowering behavior.
- No new ARC runtime helper entrypoints.
- No public runtime ABI widening.
- No conformance matrix or execution-smoke closeout yet; that belongs to `M262-E002`.

## Evidence

- `tmp/reports/m262/M262-E001/arc_runtime_gate_summary.json`
