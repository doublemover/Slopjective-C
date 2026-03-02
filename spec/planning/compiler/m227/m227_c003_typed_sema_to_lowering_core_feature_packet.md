# M227-C003 Typed Sema-to-Lowering Core Feature Packet

Packet: `M227-C003`  
Milestone: `M227`  
Lane: `C`

## Scope

Implement typed sema-to-lowering core feature accounting and determinism gates, then thread those signals into parse/lowering readiness.

## Anchors

- Contract: `docs/contracts/m227_typed_sema_to_lowering_core_feature_c003_expectations.md`
- Checker: `scripts/check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
- Typed handoff surface: `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
- Parse/lowering readiness surface: `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Shared frontend types: `native/objc3c/src/pipeline/objc3_frontend_types.h`

## Required Evidence

- `tmp/reports/m227/M227-C003/typed_sema_to_lowering_core_feature_contract_summary.json`

## Determinism Criteria

- Typed core feature case accounting is explicit and self-consistent.
- Typed handoff key + core feature key are deterministic and non-empty when ready.
- Parse/lowering readiness requires typed core feature consistency instead of raw boolean pass-through.
