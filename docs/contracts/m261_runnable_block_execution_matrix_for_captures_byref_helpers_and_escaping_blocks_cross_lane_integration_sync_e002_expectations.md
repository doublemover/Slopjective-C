# M261 Runnable Block Execution Matrix For Captures, Byref, Helpers, And Escaping Blocks Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runnable-block-execution-matrix/m261-e002-v1`

Issue: `#7193`

## Objective

Close M261 with one fail-closed lane-E execution matrix proving the currently
supported runnable block slice through real native programs and the retained
D003 runtime probe.

## Required implementation

1. Add a canonical expectations document for the runnable block execution
   matrix.
2. Add this packet, a deterministic checker, tooling tests, and a direct
   lane-E readiness runner:
   - `scripts/check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py`
   - `tests/tooling/test_check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py`
   - `scripts/run_m261_e002_lane_e_readiness.py`
3. Add `M261-E002` anchor text to:
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
4. Keep the closeout matrix fail closed over the canonical upstream evidence
   chain:
   - `tmp/reports/m261/M261-A003/block_source_storage_annotations_summary.json`
   - `tmp/reports/m261/M261-B003/byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_summary.json`
   - `tmp/reports/m261/M261-C004/escaping_block_runtime_hook_lowering_summary.json`
   - `tmp/reports/m261/M261-D003/block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.json`
   - `tmp/reports/m261/M261-E001/block_runtime_gate_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops
   reporting successful coverage, drops the frozen contract ids, or stops
   proving the retained D003 escaping-pointer-capture runtime behavior.
6. The closeout matrix must compile, link, and run the supported block fixtures:
   - `m261_owned_object_capture_runtime_positive.objc3` with exit `11`
   - `m261_nonowning_object_capture_runtime_positive.objc3` with exit `9`
   - `m261_byref_cell_copy_dispose_runtime_positive.objc3` with exit `14`
   - `m261_escaping_block_runtime_hook_argument_positive.objc3` with exit `14`
   - `m261_escaping_block_runtime_hook_return_positive.objc3` with exit `0`
7. The emitted IR must publish the closeout boundary and named metadata:
   - `; runnable_block_execution_matrix = ...`
   - `!objc3.objc_runnable_block_execution_matrix`
8. `package.json` must wire:
   - `check:objc3c:m261-e002-runnable-block-execution-matrix`
   - `test:tooling:m261-e002-runnable-block-execution-matrix`
   - `check:objc3c:m261-e002-lane-e-readiness`
9. The closeout matrix must explicitly hand off to `M262-A001`.

## Canonical models

- Evidence model:
  `a003-b003-c004-d003-e001-summary-plus-integrated-native-block-smoke-matrix`
- Active matrix model:
  `closeout-matrix-runs-owned-nonowning-byref-and-escaping-block-fixtures-against-the-native-runtime`
- Failure model:
  `fail-closed-on-runnable-block-execution-matrix-drift-or-doc-mismatch`

## Non-goals

- No new parser, sema, lowering, or runtime feature work.
- No public block-object ABI.
- No public stable block runtime helper declarations.
- No generalized foreign block ABI interop.
- No caller-frame forwarding bridge.
- No ARC or post-M261 ownership/runtime widening; that belongs to `M262`.

## Evidence

- `tmp/reports/m261/M261-E002/runnable_block_execution_matrix_summary.json`
