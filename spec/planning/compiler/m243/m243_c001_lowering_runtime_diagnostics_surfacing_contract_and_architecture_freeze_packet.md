# M243-C001 Lowering/Runtime Diagnostics Surfacing Contract and Architecture Freeze Packet

Packet: `M243-C001`
Milestone: `M243`
Lane: `C`
Dependencies: none

## Scope

Freeze lane-C lowering/runtime diagnostics surfacing anchors before broader M243
diagnostics UX and fix-it expansion work.

## Anchors

- Contract: `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_c001_expectations.md`
- Checker: `scripts/check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py`
- Frontend artifact diagnostics surfacing: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Diagnostics artifact emission: `native/objc3c/src/io/objc3_diagnostics_artifacts.cpp`
- CLI diagnostics publication: `native/objc3c/src/driver/objc3_objc3_path.cpp`
- C API diagnostics publication: `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m243/M243-C001/lowering_runtime_diagnostics_surfacing_contract_summary.json`

## Determinism Criteria

- Frontend artifact fail-closed gates must surface lowering/runtime diagnostics
  as deterministic `O3L*` payloads.
- Diagnostics artifacts must merge stage and post-pipeline diagnostics in stable
  order.
- CLI and C API entry points must keep surfaced lowering/runtime diagnostics
  observable in emitted diagnostics outputs and stage summaries.
