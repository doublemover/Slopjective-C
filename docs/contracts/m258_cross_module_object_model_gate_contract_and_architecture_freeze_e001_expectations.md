# M258 Cross-Module Object-Model Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-cross-module-object-model-gate/m258-e001-v1`

Issue: `#7166`

## Objective

Freeze one fail-closed lane-E evidence gate proving the current import/module
object-model chain is runnable across module boundaries rather than merely
modeled.

## Required implementation

1. Add a canonical expectations document for the M258 cross-module object-model
   gate.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-E
   readiness runner:
   - `scripts/check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py`
   - `scripts/run_m258_e001_lane_e_readiness.py`
3. Add `M258-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/libobjc3c_frontend/api.h`
4. Keep the gate fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m258/M258-A002/runtime_aware_import_module_frontend_closure_summary.json`
   - `tmp/reports/m258/M258-B002/imported_runtime_metadata_semantic_rules_summary.json`
   - `tmp/reports/m258/M258-C002/module_metadata_artifact_reuse_summary.json`
   - `tmp/reports/m258/M258-D002/cross_module_runtime_packaging_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops
   reporting successful check coverage, or drops the contract ids that define
   the current cross-module runnable object-model boundary.
6. `package.json` must wire:
   - `check:objc3c:m258-e001-cross-module-object-model-gate`
   - `test:tooling:m258-e001-cross-module-object-model-gate`
   - `check:objc3c:m258-e001-lane-e-readiness`
7. The gate must explicitly hand off to `M258-E002`.

## Canonical models

- Evidence model:
  `a002-b002-c002-d002-summary-chain`
- Execution gate model:
  `cross-module-runnable-object-model-evidence-consumes-import-sema-reuse-and-runtime-packaging-proofs`
- Failure model:
  `fail-closed-on-cross-module-object-model-evidence-drift`

## Non-goals

- No new parser, sema, lowering, or runtime feature work.
- No new module/import source surface.
- No new cross-module method-body binding claims beyond the current D002 proof.
- No expanded runnable execution matrix yet; that belongs to `M258-E002`.
- No public in-memory cross-module object-model ABI.

## Evidence

- `tmp/reports/m258/M258-E001/cross_module_object_model_gate_summary.json`
