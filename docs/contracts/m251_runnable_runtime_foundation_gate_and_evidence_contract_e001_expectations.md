# M251 Lane E Runnable-Runtime Foundation Gate and Evidence Contract Expectations (E001)

Contract ID: `objc3c-runnable-runtime-foundation-gate-evidence-contract/m251-e001-v1`
Status: Accepted
Scope: M251 lane-E aggregate gate that proves the native runtime foundation
exists, links, and remains the canonical execution target for the next runtime
milestones.

## Objective

Fail closed unless the aggregate M251 foundation gate keeps the completed
lane-A/lane-B/lane-C/lane-D evidence synchronized and proves that emitted
native execution now targets `artifacts/lib/objc3_runtime.lib` rather than the
test shim.

## Dependency Scope

- Dependencies: none
- Upstream evidence inputs remain mandatory:
  - `tmp/reports/m251/M251-A003/runtime_record_manifest_handoff_contract_summary.json`
  - `tmp/reports/m251/M251-B003/illegal_runtime_exposed_declaration_diagnostics_summary.json`
  - `tmp/reports/m251/M251-C003/runtime_metadata_object_inspection_harness_summary.json`
  - `tmp/reports/m251/M251-D003/runtime_support_library_link_wiring_summary.json`
  - `tmp/artifacts/objc3c-native/execution-smoke/m251_d003_runtime_library_link_wiring/summary.json`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m251/m251_e001_runnable_runtime_foundation_gate_and_evidence_contract_packet.md`
  - `scripts/check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py`
  - `tests/tooling/test_check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py`

## Required Invariants

1. Lane-E freezes one canonical aggregate gate over `M251-A003`, `M251-B003`,
   `M251-C003`, and `M251-D003`.
2. `M251-A003` evidence remains the canonical proof that runtime metadata
   source records are normalized into manifest-visible handoff data.
3. `M251-B003` evidence remains the canonical proof that illegal or incomplete
   runtime-exposed declarations fail closed with deterministic diagnostics.
4. `M251-C003` evidence remains the canonical proof that object inspection
   exposes the reserved runtime metadata sections and retained anchor symbols.
5. `M251-D003` evidence remains the canonical proof that emitted-object
   execution consumers link against `artifacts/lib/objc3_runtime.lib`.
6. The D003 smoke summary remains `PASS` and continues to report
   `runtime_library = artifacts/lib/objc3_runtime.lib`.
7. The native runtime archive `artifacts/lib/objc3_runtime.lib` must exist when
   the gate runs.
8. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicit test-only
   evidence and not the canonical runtime library.

## Non-Goals and Fail-Closed Rules

- `M251-E001` does not claim runtime metadata registration/startup is
  complete.
- `M251-E001` does not claim classes, protocols, categories, properties, or
  ivars are executable runtime entities yet.
- `M251-E001` does not claim ownership, blocks, ARC, or cross-module runtime
  semantics are complete.
- `M251-E001` exists to freeze the aggregate evidence boundary that `M252+`
  must preserve.
- The gate therefore fails closed if any upstream summary disappears, stops
  reporting `ok: true`, drops its required dynamic evidence, or if the D003
  smoke summary drifts away from `artifacts/lib/objc3_runtime.lib`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract`.
- `package.json` includes `test:tooling:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract`.
- `package.json` includes `check:objc3c:m251-e001-lane-e-readiness`.
- `package.json` keeps the upstream readiness inputs explicit:
  - `check:objc3c:m251-a003-lane-a-readiness`
  - `check:objc3c:m251-b003-lane-b-readiness`
  - `check:objc3c:m251-c003-lane-c-readiness`
  - `check:objc3c:m251-d003-lane-d-readiness`

## Validation

- `python scripts/check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py`
- `python -m pytest tests/tooling/test_check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py -q`
- `npm run check:objc3c:m251-e001-lane-e-readiness`

## Evidence Path

- `tmp/reports/m251/M251-E001/runnable_runtime_foundation_gate_contract_summary.json`
