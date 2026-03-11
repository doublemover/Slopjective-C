# M261-C001 Block Lowering ABI And Artifact Boundary Contract And Architecture Freeze Packet

Packet: `M261-C001`
Milestone: `M261`
Wave: `W53`
Lane: `C`
Issue: `#7185`
Contract ID: `objc3c-executable-block-lowering-abi-artifact-boundary/m261-c001-v1`
Dependencies: None

## Objective

Freeze the truthful lane-C block lowering ABI and artifact boundary so `M261-C002` can implement runnable block objects, invoke thunks, byref cells, and helper emission against one explicit contract.

## Canonical Boundary

- contract id `objc3c-executable-block-lowering-abi-artifact-boundary/m261-c001-v1`
- abi model `block-object-descriptor-invoke-byref-and-helper-abi-boundary-freezes-on-source-modeled-lowering-surfaces-before-runnable-emission`
- helper symbol policy `copy-dispose-and-byref-helper-symbols-remain-source-modeled-and-non-emitted-until-m261-c002`
- artifact inventory model `source-only-manifest-lowering-surfaces-plus-fail-closed-native-ir-boundary-before-runnable-block-object-artifacts`
- fail-closed model `native-emit-fails-closed-on-block-literals-before-runnable-block-object-lowering`
- non-goal model `no-emitted-block-object-records-no-invoke-thunks-no-byref-cell-storage-no-copy-dispose-helper-bodies`
- lane contracts preserved by this freeze:
  - `m166-block-literal-capture-lowering-v1`
  - `m167-block-abi-invoke-trampoline-lowering-v1`
  - `m168-block-storage-escape-lowering-v1`
  - `m169-block-copy-dispose-lowering-v1`
  - `m261-block-lowering-abi-artifact-boundary-v1`
- emitted IR comment `; executable_block_lowering_abi_artifact_boundary = ...`

## Acceptance Criteria

- Add explicit lane-C boundary constants in `native/objc3c/src/ast/objc3_ast.h`.
- Add the lane-C contract constant and summary declaration in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add one deterministic block lowering ABI/artifact boundary summary in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/parse/objc3_parser.cpp` explicit that parser does not assign runnable block artifact slots.
- Keep `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` explicit that sema hands off lowering surfaces but does not materialize runnable block artifacts.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` publish the new emitted boundary line.
- Add deterministic docs/spec/package/checker/test evidence.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/hello.objc3` proving emitted IR carries `; executable_block_lowering_abi_artifact_boundary = ...`.
2. Source-only frontend probe over `tests/tooling/fixtures/native/m261_owned_object_capture_helper_positive.objc3` proving manifests already publish the capture, invoke trampoline, storage escape, and copy/dispose lowering surfaces, with capture/invoke deterministic and storage/copy-dispose remaining truthful source-only helper/escape profiles.
3. Native compile probe over `tests/tooling/fixtures/native/m261_owned_object_capture_helper_positive.objc3` proving native block lowering still fails closed with `O3S221` and emits no manifest, IR, or object artifact.

## Non-Goals

- emitted block object records
- emitted invoke-thunk bodies
- emitted byref cell storage
- emitted copy/dispose helper bodies
- runnable block execution

## Validation Commands

- `python scripts/check_m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m261-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m261/M261-C001/block_lowering_abi_artifact_boundary_contract_summary.json`
- `M261-C002` is the explicit next issue after this freeze lands.
