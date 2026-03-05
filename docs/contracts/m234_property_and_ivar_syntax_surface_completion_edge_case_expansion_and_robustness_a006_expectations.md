# M234 Property and Ivar Syntax Surface Completion Edge-Case Expansion and Robustness Expectations (A006)

Contract ID: `objc3c-property-and-ivar-syntax-surface-completion-edge-case-expansion-and-robustness/m234-a006-v1`
Status: Accepted
Scope: M234 lane-A edge-case expansion and robustness continuity for property and ivar syntax surface completion dependency wiring.

## Objective

Fail closed unless lane-A edge-case expansion and robustness dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue: `#6617`
- Dependencies: `M234-A005`
- M234-A005 edge-case and compatibility completion anchors remain mandatory prerequisites:
  - `docs/contracts/m234_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_a005_expectations.md`
  - `spec/planning/compiler/m234/m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_contract.py`
- Packet/checker/test assets for A006 remain mandatory:
  - `spec/planning/compiler/m234/m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M234 lane-A A006 property and ivar syntax surface completion edge-case expansion and robustness anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A property and ivar syntax surface completion edge-case expansion and robustness fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A property and ivar syntax surface completion edge-case expansion and robustness metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-a006-property-and-ivar-syntax-surface-completion-edge-case-expansion-and-robustness-contract`.
- `package.json` includes
  `test:tooling:m234-a006-property-and-ivar-syntax-surface-completion-edge-case-expansion-and-robustness-contract`.
- `package.json` includes `check:objc3c:m234-a006-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m234-a006-lane-a-readiness`

## Evidence Path

- `tmp/reports/m234/M234-A006/property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_summary.json`

