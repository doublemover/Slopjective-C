# M234 Property and Ivar Syntax Surface Completion Edge-Case and Compatibility Completion Expectations (A005)

Contract ID: `objc3c-property-and-ivar-syntax-surface-completion-edge-case-and-compatibility-completion/m234-a005-v1`
Status: Accepted
Scope: M234 lane-A edge-case and compatibility completion continuity for property and ivar syntax surface completion dependency wiring.

## Objective

Fail closed unless lane-A edge-case and compatibility completion dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue: `#6616`
- Dependencies: `M234-A004`
- M234-A004 core feature expansion anchors remain mandatory prerequisites:
  - `docs/contracts/m234_property_and_ivar_syntax_surface_completion_core_feature_expansion_a004_expectations.md`
  - `spec/planning/compiler/m234/m234_a004_property_and_ivar_syntax_surface_completion_core_feature_expansion_packet.md`
  - `scripts/check_m234_a004_property_and_ivar_syntax_surface_completion_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m234_a004_property_and_ivar_syntax_surface_completion_core_feature_expansion_contract.py`
- Packet/checker/test assets for A005 remain mandatory:
  - `spec/planning/compiler/m234/m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M234 lane-A A005 property and ivar syntax surface completion edge-case and compatibility completion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A property and ivar syntax surface completion edge-case and compatibility completion fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A property and ivar syntax surface completion edge-case and compatibility completion metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-a005-property-and-ivar-syntax-surface-completion-edge-case-and-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m234-a005-property-and-ivar-syntax-surface-completion-edge-case-and-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m234-a005-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m234-a005-lane-a-readiness`

## Evidence Path

- `tmp/reports/m234/M234-A005/property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_summary.json`
