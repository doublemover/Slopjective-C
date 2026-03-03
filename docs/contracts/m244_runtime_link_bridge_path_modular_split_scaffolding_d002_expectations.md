# M244 Runtime/Link Bridge-Path Modular Split and Scaffolding Expectations (D002)

Contract ID: `objc3c-runtime-link-bridge-path-modular-split-scaffolding/m244-d002-v1`
Status: Accepted
Dependencies: `M244-D001`
Scope: lane-D runtime/link bridge-path modular split/scaffolding continuity for deterministic dependency chaining and fail-closed readiness integration.

## Objective

Execute lane-D runtime/link bridge-path modular split/scaffolding governance on
top of D001 freeze assets so downstream runtime projection and metadata
integration remain deterministic and fail-closed.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6574` defines canonical lane-D modular split/scaffolding scope.
- `M244-D001` assets remain mandatory prerequisites:
  - `docs/contracts/m244_runtime_link_bridge_path_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m244/m244_d001_runtime_link_bridge_path_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m244_d001_runtime_link_bridge_path_contract.py`
  - `tests/tooling/test_check_m244_d001_runtime_link_bridge_path_contract.py`

## Deterministic Invariants

1. lane-D modular split/scaffolding dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chaining enforces `M244-D001` before `M244-D002`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-d002-runtime-link-bridge-path-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m244-d002-runtime-link-bridge-path-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m244-d002-lane-d-readiness`.
- lane-D readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-d001-lane-d-readiness`
  - `check:objc3c:m244-d002-lane-d-readiness`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m244_d002_runtime_link_bridge_path_modular_split_scaffolding_contract.py`
- `python scripts/check_m244_d002_runtime_link_bridge_path_modular_split_scaffolding_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_d002_runtime_link_bridge_path_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m244-d002-lane-d-readiness`

## Evidence Path

- `tmp/reports/m244/M244-D002/runtime_link_bridge_path_modular_split_scaffolding_contract_summary.json`

