# M234 Property and Ivar Syntax Surface Completion Diagnostics Hardening Expectations (A007)

Contract ID: `objc3c-property-and-ivar-syntax-surface-completion-diagnostics-hardening/m234-a007-v1`
Status: Accepted
Scope: M234 lane-A diagnostics hardening continuity for property and ivar syntax surface completion dependency wiring.

## Objective

Fail closed unless lane-A diagnostics hardening dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue: `#6618`
- Dependencies: `M234-A006`
- M234-A006 edge-case expansion and robustness anchors remain mandatory prerequisites:
  - `docs/contracts/m234_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_a006_expectations.md`
  - `spec/planning/compiler/m234/m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py`
- Packet/checker/test assets for A007 remain mandatory:
  - `spec/planning/compiler/m234/m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_packet.md`
  - `scripts/check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M234 lane-A A007 property and ivar syntax surface completion diagnostics hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A property and ivar syntax surface completion diagnostics hardening fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A property and ivar syntax surface completion diagnostics hardening metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-a007-property-and-ivar-syntax-surface-completion-diagnostics-hardening-contract`.
- `package.json` includes
  `test:tooling:m234-a007-property-and-ivar-syntax-surface-completion-diagnostics-hardening-contract`.
- `package.json` includes `check:objc3c:m234-a007-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m234-a007-lane-a-readiness`

## Evidence Path

- `tmp/reports/m234/M234-A007/property_and_ivar_syntax_surface_completion_diagnostics_hardening_summary.json`

