# M261 Executable Block Object And Invoke-Thunk Lowering Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-executable-block-object-and-invoke-thunk-lowering/m261-c002-v1`
Status: Accepted
Issue: `#7186`
Scope: Land one real runnable lane-C block lowering slice for stack block objects, readonly scalar captures, and direct local invocation while leaving byref/helper/ownership-sensitive cases to `M261-C003`.

## Objective

Upgrade the frozen `M261-C001` block lowering boundary into one truthful runtime-backed capability: native lowering emits stack block storage, emits one internal invoke thunk per runnable block literal, links successfully through the existing runtime launch contract, and executes a direct local invocation path.

## Required Invariants

1. `native/objc3c/src/ast/objc3_ast.h` remains the canonical declaration point for:
   - `objc3c-executable-block-object-and-invoke-thunk-lowering/m261-c002-v1`
   - active model `native-lowering-emits-stack-block-objects-and-direct-local-invoke-thunks-for-readonly-scalar-captures`
   - deferred model `byref-cells-copy-dispose-helpers-owned-object-captures-and-heap-promotion-stay-fail-closed-until-m261-c003`
   - execution evidence model `native-compile-link-run-proves-local-block-invocation-through-emitted-block-storage-and-invoke-thunk`.
2. `native/objc3c/src/parse/objc3_parser.cpp` stays explicit that parser-owned block parameter/body ordering is retained for lowering without assigning byref/helper/runtime ownership semantics.
3. `native/objc3c/src/sema/objc3_semantic_passes.cpp` admits only the narrow runnable `M261-C002` slice:
   - normalized block literals
   - at most four parameters
   - no mutated captures
   - no byref slots
   - no copy/dispose helpers
   - no ownership-sensitive object captures.
4. `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` and `native/objc3c/src/lower/objc3_lowering_contract.cpp` keep the typed-handoff invariants truthful for readonly captures instead of forcing stale mutable/byref equality assumptions.
5. `native/objc3c/src/ir/objc3_ir_emitter.cpp` emits:
   - one stack-resident block object allocation
   - one stored invoke pointer
   - captured scalar slot stores
   - one internal invoke thunk definition
   - one indirect call through the loaded invoke pointer.
6. Native IR carries the dedicated emitted summary line:
   - `; executable_block_object_invoke_thunk_lowering = ...`

## Dynamic Coverage

1. Native compile/link/run over `tests/tooling/fixtures/native/m261_executable_block_object_invoke_thunk_positive.objc3` succeeds with:
   - compile exit `0`
   - successful link through the emitted registration manifest/runtime archive/linker flags
   - run exit `15`.
2. The positive IR proves runnable lowering by carrying:
   - `; executable_block_object_invoke_thunk_lowering = ...`
   - one stack block allocation of the form `{ ptr, [N x i32] }`
   - one stored thunk pointer `store ptr @__objc3_block_invoke...`
   - one thunk definition `define internal i32 @__objc3_block_invoke...`
   - one indirect invoke call through the loaded block function pointer.
3. The positive manifest/object path proves the current scope is honest:
   - `module.object-backend.txt` is `llvm-direct`
   - `module.obj` exists and is non-empty
   - `module.runtime-registration-manifest.json` and `module.runtime-metadata-linker-options.rsp` exist
   - block lowering surfaces remain deterministic with `byref_slot_count_total=0` and `copy_helper_required_sites=0`.
4. Deferred cases still fail closed:
   - `tests/tooling/fixtures/native/m261_capture_legality_escape_invocation_positive.objc3` fails with `O3S221`
   - `tests/tooling/fixtures/native/m261_owned_object_capture_helper_positive.objc3` fails with `O3S221`.

## Non-Goals And Fail-Closed Rules

- `M261-C002` does not emit byref cells.
- `M261-C002` does not emit copy/dispose helper bodies.
- `M261-C002` does not make ownership-qualified object captures runnable.
- `M261-C002` does not make escaping/heap-promoted block values runnable.
- If a block literal requires byref storage, helper bodies, ARC ownership handling, or heap promotion, native emit must still fail closed pending `M261-C003`.

## Architecture And Spec Anchors

- `docs/objc3c-native.md`
- `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Build And Readiness Integration

- `package.json` includes `check:objc3c:m261-c002-executable-block-object-invoke-thunk-lowering`.
- `package.json` includes `test:tooling:m261-c002-executable-block-object-invoke-thunk-lowering`.
- `package.json` includes `check:objc3c:m261-c002-lane-c-readiness`.

## Validation

- `python scripts/check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py -q`
- `npm run check:objc3c:m261-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m261/M261-C002/executable_block_object_invoke_thunk_lowering_summary.json`
