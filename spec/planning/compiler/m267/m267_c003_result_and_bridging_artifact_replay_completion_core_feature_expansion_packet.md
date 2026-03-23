# M267-C003 Result And Bridging Artifact Replay Completion Core Feature Expansion Packet

Packet: `M267-C003`
Issue: `#7276`
Milestone: `M267`
Lane: `C`
Status: Planned/Implemented
Dependencies: `M267-C002`, `M258-A002`
Next issue: `M267-D001`

## Goal

Close the replay-completion tranche for the current Part 6 lowering surface by preserving the `M267-C002` Part 6 replay packet through emitted sidecars, runtime-import artifacts, and downstream consumer replay publication.

## Truth Boundary

This issue proves artifact preservation and separate-compilation replay continuity. It does not claim:

- generalized foreign exception ABI
- live cross-module runtime execution of imported Part 6 packets
- runtime helper ABI closure beyond the current lane-C emission surface

## Required Implementation Surface

- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.h`
- `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`
- `native/objc3c/src/pipeline/objc3_runtime_import_surface.h`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.h`
- `native/objc3c/src/io/objc3_manifest_artifacts.h`
- `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`

## Required Artifact Surfaces

Provider compilation must emit:

- `module.manifest.json`
- `module.ll`
- `module.obj`
- `module.object-backend.txt`
- `module.part6-error-replay.json`
- `module.runtime-import-surface.json`

Consumer compilation must emit the same core artifact family while preserving imported provider replay keys through:

- `frontend.pipeline.semantic_surface.objc_part6_result_and_bridging_artifact_replay`
- `module.runtime-import-surface.json`
- emitted IR comment and named metadata anchors

## Proof Fixtures

Provider:

- `tests/tooling/fixtures/native/m267_c003_part6_artifact_replay_producer.objc3`

Consumer:

- `tests/tooling/fixtures/native/m267_c003_result_bridge_consumer.objc3`

The checker must compile the provider with ordinal `1` and the consumer with ordinal `2` so the cross-module registration plan remains fail-closed but non-conflicting.

## Required Dynamic Proof

1. Provider compile succeeds.
2. Provider emits sidecar and runtime-import artifacts.
3. Provider manifest semantic surface and provider sidecar agree on the canonical C003 contract.
4. Consumer compile succeeds with `--objc3-import-runtime-surface <provider/module.runtime-import-surface.json>`.
5. Consumer manifest and runtime-import artifact preserve imported provider replay keys.
6. Both provider and consumer IR carry:
   - `; part6_result_and_bridging_artifact_replay = ...`
   - `!objc3.objc_part6_result_and_bridging_artifact_replay = !{!88}`
7. `module.object-backend.txt` remains `llvm-direct` for both compiles.

## Deliverables

- expectations doc
- packet doc
- fail-closed checker
- lane readiness runner
- pytest harness
- package scripts
- summary JSON under `tmp/reports/m267/M267-C003/`
