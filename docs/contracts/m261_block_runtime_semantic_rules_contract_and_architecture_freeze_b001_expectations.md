# M261 Block Runtime Semantic Rules Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-executable-block-runtime-semantic-rules/m261-b001-v1`

## Objective

Freeze the truthful semantic-rule boundary for block literals before lane-B
starts implementing runnable capture legality, byref behavior, helper
lowering, escape behavior, or invocation semantics.

## Required implementation

1. Add a canonical expectations document for the block runtime semantic-rule
   boundary.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-B
   readiness runner:
   - `scripts/check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py`
   - `scripts/run_m261_b001_lane_b_readiness.py`
3. Add `M261-B001` anchor text to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Freeze the current semantic boundary around:
   - source-only block admission through sema
   - deterministic capture metadata validation only
   - source-owned byref/helper/escape annotations only
   - native fail-closed rejection of runnable block semantics with `O3S221`
   - `Objc3ExecutableBlockRuntimeSemanticRulesSummary()`
5. The checker must prove the boundary with three live probes:
   - `tests/tooling/fixtures/native/hello.objc3` on the native emit path to
     confirm the emitted IR carries the canonical block runtime semantic-rules
     summary line.
   - `tests/tooling/fixtures/native/m261_block_source_storage_annotations_positive.objc3`
     through `artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
     to prove the source-only semantic admission path.
   - the same block fixture through `artifacts/bin/objc3c-native.exe` to prove
     runnable block semantics still fail closed with `O3S221`.
6. `package.json` must wire:
   - `check:objc3c:m261-b001-block-runtime-semantic-rules-contract`
   - `test:tooling:m261-b001-block-runtime-semantic-rules-contract`
   - `check:objc3c:m261-b001-lane-b-readiness`
7. The contract must explicitly hand off to `M261-B002`.

## Canonical models

- Capture legality model:
  `block-runtime-semantics-freeze-capture-legality-on-deterministic-source-owned-capture-inventory`
- Storage class model:
  `block-runtime-semantics-freeze-byref-storage-as-source-annotation-only-until-runnable-byref-lowering-lands`
- Escape behavior model:
  `block-runtime-semantics-freeze-escape-shape-as-source-annotation-only-until-runnable-heap-promotion-lands`
- Helper generation model:
  `block-runtime-semantics-freeze-helper-intent-as-source-annotation-only-until-runnable-helper-lowering-lands`
- Invocation model:
  `block-runtime-semantics-freeze-block-literals-as-source-only-function-shaped-values-while-runnable-invocation-fails-closed`
- Fail-closed model:
  `block-runtime-semantics-fail-closed-on-native-emit-before-runnable-block-semantics-land`

## Non-goals

- No runnable capture legality beyond deterministic metadata checks.
- No runnable byref lowering yet.
- No copy/dispose helper lowering or helper emission yet.
- No heap promotion or block-object execution yet.
- No runnable block invocation semantics yet.

## Evidence

- `tmp/reports/m261/M261-B001/block_runtime_semantic_rules_contract_summary.json`
