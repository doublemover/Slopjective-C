# M259 Runnable Sample Surface Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-runnable-sample-surface/m259-a001-v1`

Issue: `#7208`

## Objective

Freeze the truthful canonical runnable sample surface that the project currently
treats as proof of an executable Objective-C 3 core.

## Required implementation

1. Add a canonical expectations document for the runnable sample surface.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py`
   - `scripts/run_m259_a001_lane_a_readiness.py`
3. Freeze the canonical sample surface as exactly these proof families:
   - scalar execution smoke/replay corpus rooted at
     `tests/tooling/fixtures/native/execution/positive` and
     `tests/tooling/fixtures/native/execution/negative`
   - quickstart anchor `tests/tooling/fixtures/native/hello.objc3`
   - canonical object sample rooted at
     `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_sample.objc3`
   - property/ivar/accessor matrix rooted at
     `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
   - import/module execution matrix rooted at
     `tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`
     and
     `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
4. Freeze the current truthful proof split:
   - canonical object samples are executable
   - metadata-rich object/category/protocol behavior still relies on the
     dedicated library-plus-probe path from `M256-D004`
   - property storage/accessor behavior is proven by the `M257-E002` probe path
   - import/module behavior is proven by the `M258-E002` cross-module probe path
5. Add `M259-A001` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
   - `package.json`
6. `package.json` must wire:
   - `check:objc3c:m259-a001-runnable-sample-surface`
   - `test:tooling:m259-a001-runnable-sample-surface`
   - `check:objc3c:m259-a001-lane-a-readiness`
7. The freeze must explicitly hand off to `M259-A002`.

## Canonical models

- Evidence model:
  `execution-smoke-replay-script-surface-plus-m256-d004-m257-e002-m258-e002-summary-chain`
- Sample surface model:
  `canonical-runnable-sample-surface-composes-scalar-smoke-object-property-and-import-module-proofs`
- Failure model:
  `fail-closed-on-runnable-sample-surface-drift-or-untracked-sample-claims`

## Non-goals

- No new runnable feature claims beyond the current sample surface.
- No blocks, ARC, async, throws, or actor samples.
- No fully source-bound cross-module method-body claims.
- No widening of execution smoke into metadata-rich object-model claims before
  `M259-A002`.

## Evidence

- `tmp/reports/m259/M259-A001/runnable_sample_surface_contract_summary.json`
