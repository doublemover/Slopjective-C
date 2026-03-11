# M261 Executable Block Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-executable-block-source-closure/m261-a001-v1`

## Objective

Freeze the truthful source-surface boundary for block literals before runnable
block lowering, invocation, or runtime realization begins.

## Required implementation

1. Add a canonical expectations document for the executable block source
   closure boundary.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m261_a001_executable_block_source_closure_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m261_a001_executable_block_source_closure_contract_and_architecture_freeze.py`
   - `scripts/run_m261_a001_lane_a_readiness.py`
3. Add `M261-A001` anchor text to:
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
4. Freeze the current source-surface contract around:
   - `ParseBlockLiteralExpression()`
   - `Expr::Kind::BlockLiteral`
   - `Expr::block_capture_profile`
   - `Expr::block_abi_layout_profile`
   - `Expr::block_storage_escape_profile`
   - `Expr::block_copy_dispose_profile`
   - `Expr::block_determinism_perf_baseline_profile`
   - `Objc3ExecutableBlockSourceClosureSummary()`
5. The checker must prove the boundary with two live compiles:
   - `tests/tooling/fixtures/native/hello.objc3`
   - `tests/tooling/fixtures/native/m261_executable_block_source_closure_positive.objc3`
   It must fail closed unless:
   - the generated IR for `hello.objc3` carries the canonical block source
     closure summary line
   - the block-literal fixture reaches semantic rejection with `O3S221`
   - the block-literal fixture does not regress to parser failures like
     `O3P166`, `O3P110`, `O3P104`, or `O3P100`
6. `package.json` must wire:
   - `check:objc3c:m261-a001-executable-block-source-closure-contract`
   - `test:tooling:m261-a001-executable-block-source-closure-contract`
   - `check:objc3c:m261-a001-lane-a-readiness`
7. The contract must explicitly hand off to `M261-A002`.

## Canonical models

- Source-surface model:
  `parser-owned-block-literal-source-closure-freezes-capture-abi-storage-copy-dispose-and-baseline-profiles-before-runnable-block-realization`
- Evidence model:
  `hello-ir-boundary-plus-block-literal-o3s221-fail-closed-native-probe`
- Failure model:
  `fail-closed-on-block-source-surface-drift-before-block-runtime-realization`
- Non-goal model:
  `no-block-pointer-declarator-spellings-no-explicit-byref-storage-spellings-no-block-runtime-lowering`

## Non-goals

- No block pointer declarator spellings yet.
- No explicit `__block` byref storage spellings yet.
- No block runtime lowering or invoke-trampoline emission yet.
- No block object realization, helper emission, heap promotion, or execution yet.

## Evidence

- `tmp/reports/m261/M261-A001/executable_block_source_closure_summary.json`
