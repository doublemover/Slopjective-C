# M243-C002 Lowering/Runtime Diagnostics Surfacing Modular Split and Scaffolding Packet

Packet: `M243-C002`
Milestone: `M243`
Lane: `C`
Dependencies: `M243-C001`

## Scope

Enforce modular split/scaffolding continuity for lowering/runtime diagnostics
surfacing so fail-closed diagnostics publication remains deterministic across
parse/lowering handoff boundaries before IR emission.

## Anchors

- Contract: `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_c002_expectations.md`
- Checker: `scripts/check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py`
- Scaffold header: `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h`
- Pipeline surface types: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Pipeline wiring: `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Artifact fail-closed wiring: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`
- Spec anchors:
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Required Evidence

- `tmp/reports/m243/M243-C002/lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract_summary.json`

## Determinism Criteria

- C002 scaffold closure derives from parse/lowering readiness invariants and
  does not bypass fail-closed diagnostics hardening gates.
- Artifact emission must fail closed when diagnostics surfacing scaffold
  readiness drifts.
- Replay keys and scaffold keys remain deterministic for diagnostics surfacing
  modular split evidence continuity.
