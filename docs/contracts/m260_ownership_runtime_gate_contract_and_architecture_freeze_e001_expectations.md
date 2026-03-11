# M260 Ownership Runtime Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-ownership-runtime-gate-freeze/m260-e001-v1`

Scope: `M260-E001` freezes the machine-checked gate boundary for the supported ownership runtime baseline immediately after `M260-D002`.

## Required outcomes

1. The gate advertises the supported ownership slice only:
   - strong runtime-backed property ownership
   - weak zeroing on final release
   - `@autoreleasepool` lowering and private runtime pool drain
2. The gate advertises explicit non-goals:
   - no ARC automation
   - no block ownership runtime
   - no public ownership or autoreleasepool ABI widening
3. LLVM IR publishes the gate boundary and named metadata.
4. The gate points at the existing `M260-C002`, `M260-D001`, and `M260-D002` evidence surfaces.
5. Validation evidence lands at `tmp/reports/m260/M260-E001/ownership_runtime_gate_contract_summary.json`.

## Canonical proof artifacts

- `tmp/reports/m260/M260-C002/ownership_runtime_hook_emission_summary.json`
- `tmp/reports/m260/M260-D001/runtime_memory_management_api_contract_summary.json`
- `tmp/reports/m260/M260-D002/reference_counting_weak_autoreleasepool_summary.json`
- `scripts/check_m260_e001_ownership_runtime_gate_contract_and_architecture_freeze.py`
- `tests/tooling/test_check_m260_e001_ownership_runtime_gate_contract_and_architecture_freeze.py`
- `scripts/run_m260_e001_lane_e_readiness.py`

## Truthful boundary

- This issue freezes the gate; it does not add new runtime behavior.
- `M260-E002` must exercise this exact baseline and must not claim more than it.
