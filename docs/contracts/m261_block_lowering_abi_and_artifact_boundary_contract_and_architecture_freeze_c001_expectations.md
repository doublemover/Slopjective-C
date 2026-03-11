# M261 Block Lowering ABI And Artifact Boundary Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-executable-block-lowering-abi-artifact-boundary/m261-c001-v1`
Status: Accepted
Issue: `#7185`
Scope: Freeze the truthful lane-C lowering boundary for runnable block objects without claiming that native block-object emission already exists.

## Objective

Make the current block lowering ABI and artifact boundary explicit so `M261-C002` can implement runnable block objects against one frozen contract instead of rediscovering it from historical lane-A and lane-B surfaces.

## Required Invariants

1. `native/objc3c/src/ast/objc3_ast.h` remains the canonical declaration point for:
   - `objc3c-executable-block-lowering-abi-artifact-boundary/m261-c001-v1`
   - ABI model `block-object-descriptor-invoke-byref-and-helper-abi-boundary-freezes-on-source-modeled-lowering-surfaces-before-runnable-emission`
   - helper symbol policy `copy-dispose-and-byref-helper-symbols-remain-source-modeled-and-non-emitted-until-m261-c002`
   - artifact inventory model `source-only-manifest-lowering-surfaces-plus-fail-closed-native-ir-boundary-before-runnable-block-object-artifacts`
   - fail-closed model `native-emit-fails-closed-on-block-literals-before-runnable-block-object-lowering`
   - non-goal model `no-emitted-block-object-records-no-invoke-thunks-no-byref-cell-storage-no-copy-dispose-helper-bodies`.
2. `native/objc3c/src/lower/objc3_lowering_contract.h` remains the canonical declaration point for the lane contract `m261-block-lowering-abi-artifact-boundary-v1` and the summary declaration `Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary()`.
3. `native/objc3c/src/lower/objc3_lowering_contract.cpp` publishes one deterministic boundary summary referencing the existing block lowering lane contracts for capture, invoke trampoline, storage escape, and copy/dispose lowering.
4. `native/objc3c/src/parse/objc3_parser.cpp` remains explicit that parser-owned block source facts do not assign emitted block-object layout slots, invoke thunks, byref cells, or helper symbols.
5. `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` remains explicit that sema hands lane-C one deterministic set of source-owned lowering surfaces but does not materialize runnable block artifacts.
6. `native/objc3c/src/ir/objc3_ir_emitter.cpp` publishes the dedicated emitted boundary line `; executable_block_lowering_abi_artifact_boundary = ...` on native IR paths that do emit IR.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/hello.objc3` proves emitted IR carries the dedicated block lowering ABI/artifact boundary line without requiring block syntax in the compiled program.
2. Source-only frontend probe over `tests/tooling/fixtures/native/m261_owned_object_capture_helper_positive.objc3` proves manifests already expose the four lowering surfaces that later runnable emission must preserve:
   - `objc_block_literal_capture_lowering_surface`
   - `objc_block_abi_invoke_trampoline_lowering_surface`
   - `objc_block_storage_escape_lowering_surface`
   - `objc_block_copy_dispose_lowering_surface`
   Capture and invoke surfaces remain deterministic on that corpus; storage-escape and copy/dispose surfaces remain truthful source-only helper/escape profiles and are not required to be deterministic yet.
3. Native compile probe over `tests/tooling/fixtures/native/m261_owned_object_capture_helper_positive.objc3` still fails closed with `O3S221` and emits no manifest, IR, or object artifact.

## Non-Goals And Fail-Closed Rules

- `M261-C001` does not emit block object records.
- `M261-C001` does not emit invoke-thunk bodies.
- `M261-C001` does not emit byref cell storage.
- `M261-C001` does not emit copy/dispose helper bodies.
- `M261-C001` does not make native block execution runnable.
- If this boundary drifts before `M261-C002`, the compiler must fail closed rather than silently widening block-object emission.

## Architecture And Spec Anchors

- `docs/objc3c-native.md`
- `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Build And Readiness Integration

- `package.json` includes `check:objc3c:m261-c001-block-lowering-abi-artifact-boundary-contract`.
- `package.json` includes `test:tooling:m261-c001-block-lowering-abi-artifact-boundary-contract`.
- `package.json` includes `check:objc3c:m261-c001-lane-c-readiness`.

## Validation

- `python scripts/check_m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m261-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m261/M261-C001/block_lowering_abi_artifact_boundary_contract_summary.json`
