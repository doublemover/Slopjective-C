# Runtime-Facing Type Metadata Diagnostics Hardening Expectations (M227-D007)

Contract ID: `objc3c-runtime-facing-type-metadata-diagnostics-hardening/m227-d007-v1`
Status: Accepted
Dependencies: `M227-D006`
Scope: Lane-D runtime-facing type metadata diagnostics hardening dependency continuity for deterministic fail-closed readiness integration.

## Objective

Execute issue `#5153` by enforcing lane-D runtime-facing type metadata
diagnostics hardening governance on top of D006 edge-case expansion and
robustness assets so dependency continuity and readiness evidence remain
deterministic and fail closed.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5153` defines canonical lane-D diagnostics hardening scope.
- `M227-D006` assets remain mandatory prerequisites:
  - `docs/contracts/m227_runtime_facing_type_metadata_edge_case_expansion_and_robustness_d006_expectations.md`
  - `spec/planning/compiler/m227/m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py`

## Deterministic Invariants

1. Runtime-facing metadata diagnostics hardening fields remain explicit and
   fail closed in typed sema-to-lowering and parse/lowering readiness surfaces.
2. Diagnostics hardening consistency/readiness/key continuity remains
   deterministic and fails closed when `M227-D006` dependency references drift.
3. Lane-D readiness command chaining runs direct `M227-D006` checker/test
   evidence before `M227-D007` checker/test evidence.
4. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
5. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract`
  - `test:tooling:m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract`
  - `check:objc3c:m227-d007-lane-d-readiness`
- lane-D readiness chaining remains deterministic and fail closed:
  - `npm run check:objc3c:m227-d006-lane-d-readiness`
  - `check:objc3c:m227-d007-lane-d-readiness`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py -q`
- `python scripts/check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py`
- `python scripts/check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m227-d007-lane-d-readiness`

## Evidence Path

- `tmp/reports/m227/M227-D007/runtime_facing_type_metadata_diagnostics_hardening_contract_summary.json`
