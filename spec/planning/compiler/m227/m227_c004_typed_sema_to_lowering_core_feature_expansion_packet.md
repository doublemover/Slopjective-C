# M227-C004 Typed Sema-to-Lowering Core Feature Expansion Packet

Packet: `M227-C004`  
Milestone: `M227`  
Lane: `C`

## Scope

Implement typed sema-to-lowering core-feature expansion accounting for protocol/category, class-protocol-category linking, selector normalization, and property-attribute deterministic handoffs, then require those signals in parse/lowering readiness.

## Anchors

- Contract: `docs/contracts/m227_typed_sema_to_lowering_core_feature_expansion_c004_expectations.md`
- Checker: `scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
- Typed handoff surface: `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
- Parse/lowering readiness surface: `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Shared frontend types: `native/objc3c/src/pipeline/objc3_frontend_types.h`

## Required Evidence

- `tmp/reports/m227/M227-C004/typed_sema_to_lowering_core_feature_expansion_contract_summary.json`

## Determinism Criteria

- Expansion case accounting uses explicit pass/fail counts and deterministic expansion keying.
- Core typed-lowering consistency requires expansion consistency and non-empty expansion key.
- Parse/lowering readiness recognizes expansion fields as first-class typed-surface signals.
