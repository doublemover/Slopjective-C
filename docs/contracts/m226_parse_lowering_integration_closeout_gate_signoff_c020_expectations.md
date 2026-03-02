# M226 Parse-Lowering Integration Closeout and Gate Sign-Off Expectations (C020)

Contract ID: `objc3c-parse-lowering-integration-closeout-gate-signoff-contract/m226-c020-v1`
Status: Accepted
Scope: Parse/lowering integration closeout and gate sign-off synchronization in `native/objc3c/src/pipeline/*`.

## Objective

Expand C019 advanced integration closure with explicit integration closeout and
gate sign-off consistency/readiness evidence so parse lowering fails closed when
advanced integration continuity or closeout sign-off key determinism drifts.

## Required Invariants

1. Readiness surface tracks integration closeout sign-off evidence:
   - `toolchain_runtime_ga_operations_integration_closeout_signoff_consistent`
   - `toolchain_runtime_ga_operations_integration_closeout_signoff_ready`
   - `toolchain_runtime_ga_operations_integration_closeout_signoff_key`
2. Readiness builder computes integration closeout sign-off deterministically:
   - `IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffKey(...)`
3. Closeout sign-off key evidence is folded back into integration-closeout and
   parse-lowering guardrail key surfaces.
4. Integration closeout and gate sign-off remain fail-closed and require
   closeout sign-off consistency/readiness.
5. Readiness failure reasons include:
   - `toolchain/runtime GA operations integration closeout and sign-off is inconsistent`
   - `toolchain/runtime GA operations integration closeout and sign-off is not ready`
6. Manifest projection includes integration closeout sign-off fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c020_parse_lowering_integration_closeout_gate_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c020_parse_lowering_integration_closeout_gate_signoff_contract.py -q`
- `python scripts/check_m226_c019_parse_lowering_advanced_integration_workpack_shard1_contract.py`

## Evidence Path

- `tmp/reports/m226/m226_c020_parse_lowering_integration_closeout_gate_signoff_contract_summary.json`
