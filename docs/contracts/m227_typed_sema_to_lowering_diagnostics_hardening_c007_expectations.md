# M227 Typed Sema-to-Lowering Diagnostics Hardening Expectations (C007)

Contract ID: `objc3c-typed-sema-to-lowering-diagnostics-hardening/m227-c007-v1`
Status: Accepted
Scope: typed sema-to-lowering diagnostics hardening on top of C006 edge-case robustness.

## Objective

Execute issue `#5127` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with diagnostics-hardening consistency and readiness
invariants, plus deterministic fail-closed alignment checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C006`
- `M227-C006` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_edge_case_expansion_and_robustness_c006_expectations.md`
  - `scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
  - `spec/planning/compiler/m227/m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries diagnostics-hardening
   fields:
   - `typed_diagnostics_hardening_consistent`
   - `typed_diagnostics_hardening_ready`
   - `typed_diagnostics_hardening_key`
2. Typed diagnostics-hardening readiness requires edge-case robustness
   continuity and deterministic typed handoff inputs.
3. `Objc3ParseLoweringReadinessSurface` maps typed diagnostics-hardening fields:
   - `typed_sema_diagnostics_hardening_consistent`
   - `typed_sema_diagnostics_hardening_ready`
   - `typed_sema_diagnostics_hardening_key`
4. Parse/lowering readiness fails closed when typed diagnostics-hardening
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c007-typed-sema-to-lowering-diagnostics-hardening-contract`
  - `test:tooling:m227-c007-typed-sema-to-lowering-diagnostics-hardening-contract`
  - `check:objc3c:m227-c007-lane-c-readiness`
- lane-C readiness chaining preserves C006 continuity:
  - `scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
  - `check:objc3c:m227-c007-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-C C007
  diagnostics hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C C007 fail-closed
  diagnostics hardening governance text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C C007
  diagnostics-hardening metadata anchors.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
- `python scripts/check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m227-c007-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C007/typed_sema_to_lowering_diagnostics_hardening_contract_summary.json`
