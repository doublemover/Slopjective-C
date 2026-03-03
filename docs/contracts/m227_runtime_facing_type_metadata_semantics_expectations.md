# Runtime-Facing Type Metadata Semantics Contract and Architecture Freeze Expectations (M227-D001)

Contract ID: `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`
Status: Accepted
Scope: Runtime-facing type metadata semantics freeze across sema, pipeline transport, artifact metadata projection, and lane-D readiness wiring.

## Objective

Fail closed unless runtime-facing type metadata semantics remain explicit,
deterministic, and traceable across sema parity surfaces, pipeline handoff
transport, artifact/IR metadata projection, and architecture/spec/readiness
anchors.

## Dependency Scope

- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m227/m227_d001_runtime_facing_type_metadata_semantics_contract_freeze.md`
  - `scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
  - `tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`

## Code, Architecture, and Spec Anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- `native/objc3c/src/pipeline/frontend_pipeline_contract.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-D D001
  architecture freeze text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes runtime-facing type metadata
  semantics governance wording for `M227-D001`.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-D
  runtime-facing type metadata metadata anchors for `M227-D001`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m227-d001-runtime-facing-type-metadata-semantics-contract`.
- `package.json` includes
  `test:tooling:m227-d001-runtime-facing-type-metadata-semantics-contract`.
- `package.json` includes `check:objc3c:m227-d001-lane-d-readiness`.

## Deterministic Invariants

| ID | Requirement |
| --- | --- |
| `M227-D001-INV-01` | Runtime-facing canonical type-form anchors remain explicit and deterministic in `objc3_sema_contract.h` (`kObjc3CanonicalReferenceTypeForms`, `IsObjc3CanonicalMessageSendTypeForm(...)`, and sema/runtime metadata summary carriers). |
| `M227-D001-INV-02` | Runtime dispatch symbol defaults remain architecture-pinned and consistent between sema and pipeline contract layers (`objc3_msgsend_i32`) with no hidden drift between `kObjc3RuntimeShimHostLinkDefaultDispatchSymbol` and `kRuntimeDispatchDefaultSymbol`. |
| `M227-D001-INV-03` | `Objc3SemaParityContractSurface` preserves runtime-facing deterministic gates and readiness invariants for runtime-shim host-link and retain/release lowering summaries. |
| `M227-D001-INV-04` | Frontend pipeline transport preserves `sema_type_metadata_handoff`, parity surface wiring, and deterministic runtime-facing derived summaries (object-pointer/nullability/generics and symbol-graph/scope-resolution). |
| `M227-D001-INV-05` | Artifact projection preserves runtime-facing manifest + IR metadata projection of deterministic sema/type metadata, runtime-shim host-link, retain/release lowering, object-pointer profile, and symbol-graph scope-resolution handoff key. |
| `M227-D001-INV-06` | Architecture/spec metadata and package lane-D readiness anchors remain explicit and fail closed on drift. |
| `M227-D001-INV-07` | The D001 checker and tooling tests remain fail-closed canonical freeze gates and emit deterministic JSON summary artifacts under `tmp/reports/m227/`. |

## Validation

- `python scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py -q`
- `npm run check:objc3c:m227-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m227/M227-D001/runtime_type_metadata_semantics_contract_summary.json`
