# Toolchain/Runtime GA Operations Readiness Contract Freeze Expectations (M250-D001)

Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-freeze/m250-d001-v1`
Status: Accepted
Scope: native toolchain/runtime operation surfaces (`native/objc3c/src/io/*`, `scripts/*`) required for GA readiness.

## Objective

Freeze lane-D toolchain/runtime operational contracts so direct LLVM object-emission routing, capability probing, and replay-proof gates remain deterministic and fail-closed for GA readiness.

## Deterministic Invariants

1. `RunIRCompileLLVMDirect(...)` remains a true direct LLVM object-emission path:
   - uses `llc -filetype=obj`
   - fail-closed when `llc` is unavailable or exits non-zero
2. LLVM capability probing remains authoritative for lane-D routing gates:
   - clang and llc presence + llc obj-file support are independently probed
   - parity readiness derives from deterministic capability synthesis
   - `scripts/probe_objc3c_llvm_capabilities.py` remains the canonical capability probe entrypoint
3. Compile driver contract enforces capability-route invariants:
   - `--objc3-route-backend-from-capabilities` requires `--llvm-capabilities-summary`
   - only `clang` and `llvm-direct` backend modes are accepted
4. Deterministic replay gates remain hard blockers:
   - compile proof verifies manifest/diagnostics/IR replay identity
   - execution replay proof verifies canonical summary hash stability
5. Lane-D architecture ownership remains explicit in `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m250_d001_toolchain_runtime_ga_operations_readiness_contract.py`
- `python -m pytest tests/tooling/test_check_m250_d001_toolchain_runtime_ga_operations_readiness_contract.py -q`
- `npm run check:objc3c:m250-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m250/M250-D001/toolchain_runtime_ga_operations_readiness_contract_summary.json`
