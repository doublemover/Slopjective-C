# M261 Runnable Block Runtime Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-runnable-block-runtime-gate/m261-e001-v1`

Issue: `#7192`

## Objective

Freeze one fail-closed lane-E gate proving the current runnable block slice is
backed by the native lowering and runtime path rather than metadata-only
summaries.

## Required implementation

1. Add a canonical expectations document for the runnable block-runtime gate.
2. Add this packet, a deterministic checker, tooling tests, and a direct
   lane-E readiness runner:
   - `scripts/check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py`
   - `scripts/run_m261_e001_lane_e_readiness.py`
3. Add `M261-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Keep the gate fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m261/M261-A003/block_source_storage_annotations_summary.json`
   - `tmp/reports/m261/M261-B003/byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_summary.json`
   - `tmp/reports/m261/M261-C004/escaping_block_runtime_hook_lowering_summary.json`
   - `tmp/reports/m261/M261-D003/block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops
   reporting successful coverage, drops the frozen contract ids, or stops
   proving that runtime-backed escaping blocks execute through the native path.
6. The emitted IR must publish the gate boundary and named metadata:
   - `; runnable_block_runtime_gate = ...`
   - `!objc3.objc_runnable_block_runtime_gate`
7. `package.json` must wire:
   - `check:objc3c:m261-e001-runnable-block-runtime-gate`
   - `test:tooling:m261-e001-runnable-block-runtime-gate`
   - `check:objc3c:m261-e001-lane-e-readiness`
8. The gate must explicitly hand off to `M261-E002`.

## Canonical models

- Evidence model:
  `a003-b003-c004-d003-summary-chain`
- Active gate model:
  `runnable-block-gate-consumes-source-sema-lowering-and-runtime-proofs-rather-than-metadata-only-summaries`
- Failure model:
  `fail-closed-on-runnable-block-runtime-evidence-drift`

## Non-goals

- No new parser, sema, lowering, or runtime feature work.
- No public block-object ABI.
- No public stable block runtime helper declarations.
- No generalized foreign block ABI interop.
- No caller-frame forwarding bridge.
- No runnable closeout matrix yet; that belongs to `M261-E002`.

## Evidence

- `tmp/reports/m261/M261-E001/block_runtime_gate_summary.json`
