# M227-D003 Runtime-Facing Type Metadata Core Feature Packet

Packet: `M227-D003`
Milestone: `M227`
Lane: `D`

## Scope

Implement runtime-facing type metadata core feature case accounting and deterministic gating across typed sema-to-lowering and parse/lowering readiness surfaces.

## Anchors

- Contract: `docs/contracts/m227_runtime_facing_type_metadata_core_feature_d003_expectations.md`
- Checker: `scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`
- Shared frontend types: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Typed sema-to-lowering contract surface: `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
- Parse/lowering readiness surface: `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`

## Required Evidence

- `tmp/reports/m227/M227-D003/runtime_facing_type_metadata_core_feature_contract_summary.json`

## Determinism Criteria

- Runtime-facing deterministic handoff signals (protocol/category, class/protocol/category linking, selector normalization, property attributes) are explicitly represented on typed and readiness surfaces.
- Typed expansion case accounting uses a pinned four-case contract and yields deterministic expansion replay keys.
- Parse/lowering readiness requires expansion consistency and non-empty expansion replay keys before declaring typed core feature readiness.
- Failure paths remain fail-closed with explicit runtime-facing drift reasons.
