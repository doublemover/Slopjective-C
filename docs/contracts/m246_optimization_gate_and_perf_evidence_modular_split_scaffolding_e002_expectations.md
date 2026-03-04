# M246 Optimization Gate and Perf Evidence Modular Split/Scaffolding Expectations (E002)

Contract ID: `objc3c-optimization-gate-perf-evidence-modular-split-scaffolding/m246-e002-v1`
Status: Accepted
Scope: M246 lane-E modular split/scaffolding continuity for optimization gate and perf evidence dependency wiring.
Issue: `#6693`
Dependencies: `M246-E001`, `M246-A002`, `M246-B002`, `M246-C004`, `M246-D002`

## Objective

Fail closed unless M246 lane-E modular split/scaffolding dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Prerequisite Dependency Matrix

| Lane Task | Required Freeze State |
| --- | --- |
| `M246-E001` | Contract assets for E001 are required and must remain present/readable. |
| `M246-A002` | Contract assets for A002 are required and must remain present/readable. |
| `M246-B002` | Dependency token `M246-B002` is mandatory as pending seeded lane-B modular split/scaffolding assets. |
| `M246-C004` | Dependency token `M246-C004` is mandatory as pending seeded lane-C expansion assets. |
| `M246-D002` | Dependency token `M246-D002` is mandatory as pending seeded lane-D modular split/scaffolding assets. |

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes lane-E modular split/scaffolding dependency anchor text with `M246-E001`, `M246-A002`, `M246-B002`, `M246-C004`, and `M246-D002`.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-E optimization gate and perf evidence modular split/scaffolding fail-closed wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-E optimization gate and perf evidence modular split/scaffolding dependency anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m246-e002-optimization-gate-perf-evidence-modular-split-scaffolding-contract`.
- `package.json` includes `test:tooling:m246-e002-optimization-gate-perf-evidence-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m246-e002-lane-e-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m246_e002_optimization_gate_and_perf_evidence_modular_split_scaffolding_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m246_e002_optimization_gate_and_perf_evidence_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m246-e002-lane-e-readiness`

## Evidence Path

- `tmp/reports/m246/M246-E002/optimization_gate_perf_evidence_modular_split_scaffolding_summary.json`
