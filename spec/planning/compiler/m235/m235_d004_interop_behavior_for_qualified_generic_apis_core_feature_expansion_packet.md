# M235-D004 Interop Behavior for Qualified Generic APIs Core Feature Expansion Packet

Packet: `M235-D004`
Milestone: `M235`
Lane: `D`
Issue: `#5834`
Freeze date: `2026-03-05`
Dependencies: `M235-D003`

## Purpose

Freeze lane-D core feature expansion prerequisites for M235 interop
behavior for qualified generic APIs continuity so dependency wiring remains
deterministic and fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_interop_behavior_for_qualified_generic_apis_core_feature_expansion_d004_expectations.md`
- Checker:
  `scripts/check_m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract.py`
- Dependency anchors from `M235-D003`:
  - `docs/contracts/m235_interop_behavior_for_qualified_generic_apis_core_feature_implementation_d003_expectations.md`
  - `spec/planning/compiler/m235/m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_packet.md`
  - `scripts/check_m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m235-d003-interop-behavior-for-qualified-generic-apis-core-feature-implementation-contract`
  - `test:tooling:m235-d003-interop-behavior-for-qualified-generic-apis-core-feature-implementation-contract`
  - `check:objc3c:m235-d004-interop-behavior-for-qualified-generic-apis-core-feature-expansion-contract`
  - `test:tooling:m235-d004-interop-behavior-for-qualified-generic-apis-core-feature-expansion-contract`
  - `check:objc3c:m235-d003-lane-d-readiness`
  - `check:objc3c:m235-d004-lane-d-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `npm run check:objc3c:m235-d003-lane-d-readiness`
- `python scripts/check_m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m235-d004-lane-d-readiness`

## Evidence Output

- `tmp/reports/m235/M235-D004/interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract_summary.json`


