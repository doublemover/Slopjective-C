# M251 Lane E Cross-Lane Runtime-Foundation Gate and Bootstrap Proof Expectations (E002)

Contract ID: `objc3c-cross-lane-runtime-foundation-gate-bootstrap-proof/m251-e002-v1`
Status: Accepted
Scope: M251 lane-E integrated runtime-foundation gate proving that source
records, semantic diagnostics, object inspection, and runtime-linked execution
line up on the same native toolchain path.

## Objective

Fail closed unless the native runtime-foundation work completed in `M251-A003`,
`M251-B003`, `M251-C003`, `M251-D003`, and `M251-E001` can be exercised as one
real integrated path through `artifacts/bin/objc3c-native.exe` and the live
runtime archive.

## Dependency Scope

- Dependencies:
  - `M251-A003`
  - `M251-B003`
  - `M251-C003`
  - `M251-D003`
  - `M251-E001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m251/m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof_packet.md`
  - `scripts/check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`
  - `tests/tooling/test_check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`

## Required Invariants

1. `M251-E001` remains the canonical aggregate dependency gate for this issue.
2. The integrated gate uses real `objc3c-native` invocations and must not rely
   on mock-only substitutes.
3. A metadata-rich native compile preserves `module.manifest.json` and
   `runtime_metadata_source_records` on the integrated path.
4. An incomplete runtime-export unit fails closed with precise `O3S260`
   diagnostics on the integrated path.
5. A zero-descriptor native object-emission probe succeeds and the emitted
   object is inspected with `llvm-readobj --sections` and
   `llvm-objdump --syms`.
6. A fresh smoke run under run id `m251_e002_cross_lane_runtime_foundation_gate`
   reports `status = PASS` and `runtime_library = artifacts/lib/objc3_runtime.lib`.
7. The canonical runtime archive `artifacts/lib/objc3_runtime.lib` exists when
   the gate runs.

## Non-Goals and Fail-Closed Rules

- `M251-E002` does not claim runtime registration/startup is complete.
- `M251-E002` does not claim classes, protocols, categories, properties, or
  ivars are executable runtime objects yet.
- `M251-E002` does not replace the need for `M251-E003` operator runbooks.
- The gate therefore fails closed if any dependency summary disappears, if any
  integrated probe regresses, or if the smoke replay stops linking against the
  native runtime archive.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m251-e002-cross-lane-runtime-foundation-gate-and-bootstrap-proof`.
- `package.json` includes `test:tooling:m251-e002-cross-lane-runtime-foundation-gate-and-bootstrap-proof`.
- `package.json` includes `check:objc3c:m251-e002-lane-e-readiness`.
- `package.json` keeps the upstream lane-E dependency explicit through
  `check:objc3c:m251-e001-lane-e-readiness`.

## Validation

- `python scripts/check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`
- `python -m pytest tests/tooling/test_check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py -q`
- `npm run check:objc3c:m251-e002-lane-e-readiness`

## Evidence Path

- `tmp/reports/m251/M251-E002/cross_lane_runtime_foundation_gate_summary.json`
