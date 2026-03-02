# Runtime-Facing Type Metadata Semantics Contract and Architecture Freeze Expectations (M227-D001)

Contract ID: `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`

## Scope

M227-D001 freezes runtime-facing type metadata semantics so sema parity, pipeline transport, and artifact/runtime projection remain deterministic and fail closed while follow-on M227 shards land.

## Scope Anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- `native/objc3c/src/pipeline/frontend_pipeline_contract.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Deterministic Invariants

| ID | Requirement |
| --- | --- |
| `M227-D001-INV-01` | Runtime-facing canonical type-form anchors remain explicit and deterministic in `objc3_sema_contract.h` (`kObjc3CanonicalReferenceTypeForms`, `IsObjc3CanonicalMessageSendTypeForm(...)`, and sema/runtime metadata summary carriers). |
| `M227-D001-INV-02` | Runtime dispatch symbol defaults remain architecture-pinned and consistent between sema and pipeline contract layers (`objc3_msgsend_i32`) with no hidden drift between `kObjc3RuntimeShimHostLinkDefaultDispatchSymbol` and `kRuntimeDispatchDefaultSymbol`. |
| `M227-D001-INV-03` | `Objc3SemaParityContractSurface` preserves runtime-facing deterministic gates and readiness invariants for runtime-shim host-link and retain/release lowering summaries. |
| `M227-D001-INV-04` | Frontend pipeline transport preserves `sema_type_metadata_handoff`, parity surface wiring, and deterministic runtime-facing derived summaries (object-pointer/nullability/generics and symbol-graph/scope-resolution). |
| `M227-D001-INV-05` | Artifact projection preserves runtime-facing manifest + IR metadata projection of deterministic sema/type metadata, runtime-shim host-link, retain/release lowering, object-pointer profile, and symbol-graph scope-resolution handoff key. |
| `M227-D001-INV-06` | The D001 checker and tooling tests remain fail-closed canonical freeze gates and emit deterministic JSON summary artifacts under `tmp/reports/m227/`. |

## Acceptance Checks

- `python scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py -q`

The checker writes deterministic JSON output to `tmp/reports/m227/M227-D001/runtime_type_metadata_semantics_contract_summary.json` by default and exits non-zero on drift.
