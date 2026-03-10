# M257 Property And Ivar Execution Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-executable-property-ivar-execution-gate/m257-e001-v1`

Issue: `#7156`

## Objective

Freeze one fail-closed lane-E evidence gate proving the current M257 property,
ivar, accessor, and reflective runtime surface is executable rather than merely
modeled.

## Required implementation

1. Add a canonical expectations document for the M257 property/ivar execution gate.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-E
   readiness runner:
   - `scripts/check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py`
   - `scripts/run_m257_e001_lane_e_readiness.py`
3. Add `M257-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Keep the gate fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m257/M257-A002/property_ivar_source_model_completion_summary.json`
   - `tmp/reports/m257/M257-B003/accessor_legality_attribute_interactions_summary.json`
   - `tmp/reports/m257/M257-C003/synthesized_accessor_property_lowering_summary.json`
   - `tmp/reports/m257/M257-D003/property_metadata_reflection_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops
   reporting successful check coverage, or drops the contract ids that define
   the current executable property/ivar boundary.
6. `package.json` must wire:
   - `check:objc3c:m257-e001-property-and-ivar-execution-gate`
   - `test:tooling:m257-e001-property-and-ivar-execution-gate`
   - `check:objc3c:m257-e001-lane-e-readiness`
7. The gate must explicitly hand off to `M257-E002`.

## Canonical models

- Evidence model:
  `a002-b003-c003-d003-summary-chain`
- Execution gate model:
  `runnable-property-ivar-evidence-consumes-source-sema-lowering-and-runtime-proofs`
- Failure model:
  `fail-closed-on-property-ivar-execution-evidence-drift`

## Non-goals

- No new parser, sema, lowering, or runtime feature work.
- No new runnable sample matrix yet; that belongs to `M257-E002`.
- No new property storage or accessor realization behavior.
- No public runtime reflection ABI.

## Evidence

- `tmp/reports/m257/M257-E001/property_ivar_execution_gate_summary.json`
