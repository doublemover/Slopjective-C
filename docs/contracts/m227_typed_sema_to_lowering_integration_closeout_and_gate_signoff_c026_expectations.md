# M227 Typed Sema-to-Lowering Integration Closeout and Gate Sign-Off Expectations (C026)

Contract ID: `objc3c-typed-sema-to-lowering-integration-closeout-and-gate-signoff/m227-c026-v1`
Status: Accepted
Scope: lane-C typed sema-to-lowering integration closeout and gate sign-off on top of C025 advanced integration shard 2.

## Objective

Execute issue `#5146` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with integration-closeout/sign-off
consistency/readiness invariants, with deterministic fail-closed alignment
checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C025`
- `M227-C025` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_advanced_integration_shard2_c025_expectations.md`
  - `scripts/check_m227_c025_typed_sema_to_lowering_advanced_integration_shard2_contract.py`
  - `tests/tooling/test_check_m227_c025_typed_sema_to_lowering_advanced_integration_shard2_contract.py`
  - `spec/planning/compiler/m227/m227_c025_typed_sema_to_lowering_advanced_integration_shard2_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed integration closeout/sign-off fields:
   - `typed_integration_closeout_signoff_consistent`
   - `typed_integration_closeout_signoff_ready`
   - `typed_integration_closeout_signoff_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed integration closeout/sign-off fields:
   - `typed_sema_integration_closeout_signoff_consistent`
   - `typed_sema_integration_closeout_signoff_ready`
   - `typed_sema_integration_closeout_signoff_key`
3. Parse/lowering readiness fails closed when typed integration closeout/sign-off
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c026-typed-sema-to-lowering-integration-closeout-and-gate-signoff-contract`
  - `test:tooling:m227-c026-typed-sema-to-lowering-integration-closeout-and-gate-signoff-contract`
  - `check:objc3c:m227-c026-lane-c-readiness`

## Validation

- `python scripts/check_m227_c025_typed_sema_to_lowering_advanced_integration_shard2_contract.py`
- `python scripts/check_m227_c026_typed_sema_to_lowering_integration_closeout_and_gate_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c026_typed_sema_to_lowering_integration_closeout_and_gate_signoff_contract.py -q`
- `npm run check:objc3c:m227-c026-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C026/integration_closeout_and_gate_signoff_contract_summary.json`

