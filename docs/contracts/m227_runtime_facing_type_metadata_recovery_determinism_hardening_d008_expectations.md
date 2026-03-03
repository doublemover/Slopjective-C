# Runtime-Facing Type Metadata Recovery and Determinism Hardening Expectations (M227-D008)

Contract ID: `objc3c-runtime-facing-type-metadata-recovery-determinism-hardening/m227-d008-v1`
Status: Accepted
Dependencies: `M227-D007`
Scope: Lane-D runtime-facing type metadata recovery and determinism hardening dependency continuity for deterministic fail-closed readiness integration.

## Objective

Execute issue `#5154` by enforcing lane-D runtime-facing type metadata
recovery and determinism hardening governance on top of D007 diagnostics hardening and
robustness assets so dependency continuity and readiness evidence remain
deterministic and fail closed.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5154` defines canonical lane-D recovery and determinism hardening scope.
- `M227-D007` assets remain mandatory prerequisites:
  - `docs/contracts/m227_runtime_facing_type_metadata_diagnostics_hardening_d007_expectations.md`
  - `spec/planning/compiler/m227/m227_d007_runtime_facing_type_metadata_diagnostics_hardening_packet.md`
  - `scripts/check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py`

## Deterministic Invariants

1. Runtime-facing metadata recovery and determinism hardening fields remain explicit and
   fail closed in typed sema-to-lowering and parse/lowering readiness surfaces.
2. Recovery/determinism consistency/readiness/key continuity remains
   deterministic and fails closed when `M227-D007` dependency references drift.
3. Lane-D readiness command chaining runs direct `M227-D007` checker/test
   evidence before `M227-D008` checker/test evidence.
4. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
5. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-d008-runtime-facing-type-metadata-recovery-determinism-hardening-contract`
  - `test:tooling:m227-d008-runtime-facing-type-metadata-recovery-determinism-hardening-contract`
  - `check:objc3c:m227-d008-lane-d-readiness`
- lane-D readiness chaining remains deterministic and fail closed:
  - `npm run check:objc3c:m227-d007-lane-d-readiness`
  - `check:objc3c:m227-d008-lane-d-readiness`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py -q`
- `python scripts/check_m227_d008_runtime_facing_type_metadata_recovery_determinism_hardening_contract.py`
- `python scripts/check_m227_d008_runtime_facing_type_metadata_recovery_determinism_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m227_d008_runtime_facing_type_metadata_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m227-d008-lane-d-readiness`

## Evidence Path

- `tmp/reports/m227/M227-D008/runtime_facing_type_metadata_recovery_determinism_hardening_contract_summary.json`
