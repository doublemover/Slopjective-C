# Typed Sema-to-Lowering Contract and Architecture Freeze Expectations (M227-C001)

Contract ID: `objc3c-typed-sema-to-lowering-contract/m227-c001-v1`

## Scope

M227-C001 freezes Lane C sema-to-lowering typed contract surfaces so semantic model expansion and type-system correctness flow into lowering metadata deterministically and fail closed.

## Scope Anchors

- `native/objc3c/src/pipeline/frontend_pipeline_contract.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native/src/30-semantics.md`
- `docs/objc3c-native/src/50-artifacts.md`

## Deterministic Invariants

| ID | Requirement |
| --- | --- |
| `M227-C001-INV-01` | `native/objc3c/src/pipeline/frontend_pipeline_contract.h` preserves typed stage-level sema/lowering carrier structures (`FunctionSignatureSurface`, `SemaStageOutput`, and lowering dispatch boundaries) with fail-closed error model defaults. |
| `M227-C001-INV-02` | `native/objc3c/src/pipeline/objc3_frontend_types.h` preserves typed sema-to-lowering handoff surfaces on `Objc3FrontendPipelineResult` (`integration_surface`, `sema_type_metadata_handoff`, object-pointer/nullability/generics summary, symbol-graph/scope-resolution summary, and `sema_parity_surface`). |
| `M227-C001-INV-03` | `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` preserves canonical sema orchestration through `RunObjc3SemaPassManager(...)`, transport of integration and type-metadata handoff outputs, deterministic summary derivation, and parse/lowering readiness projection. |
| `M227-C001-INV-04` | `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` preserves typed lowering contract construction from sema/type carriers and deterministic projection into manifest and IR frontend metadata (`deterministic_type_metadata_handoff`, symbol-graph/scope-resolution handoff flags, and handoff key). |
| `M227-C001-INV-05` | `docs/objc3c-native/src/30-semantics.md` and `docs/objc3c-native/src/50-artifacts.md` continue documenting the same sema transport and lowering metadata anchors consumed by this freeze gate. |
| `M227-C001-INV-06` | The fail-closed checker and tooling tests remain canonical freeze gates for M227-C001 and write deterministic JSON summaries under `tmp/reports/m227/`. |

## Acceptance Checks

- `python scripts/check_m227_c001_typed_sema_to_lowering_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py -q`

The checker writes deterministic JSON output to `tmp/reports/m227/m227_c001_typed_sema_to_lowering_contract_summary.json` by default and exits non-zero on drift.
