# M256 Class, Protocol, And Category Conformance Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-executable-class-protocol-category-conformance-gate/m256-e001-v1`

## Objective

Freeze one fail-closed lane-E evidence gate proving the current M256 class,
protocol, and category surface is executable rather than merely modeled.

## Required implementation

1. Add a canonical expectations document for the M256 conformance gate.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-E
   readiness runner:
   - `scripts/check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py`
   - `scripts/run_m256_e001_lane_e_readiness.py`
3. Add `M256-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Keep the gate fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m256/M256-A003/protocol_category_source_surface_completion_for_executable_runtime_summary.json`
   - `tmp/reports/m256/M256-B004/inheritance_override_realization_legality_summary.json`
   - `tmp/reports/m256/M256-C003/realization_records_summary.json`
   - `tmp/reports/m256/M256-D004/canonical_runnable_object_sample_support_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops
   reporting `ok: true`, or drops the contract ids that define the current
   executable class/protocol/category boundary.
6. `package.json` must wire:
   - `check:objc3c:m256-e001-class-protocol-and-category-conformance-gate`
   - `test:tooling:m256-e001-class-protocol-and-category-conformance-gate`
   - `check:objc3c:m256-e001-lane-e-readiness`
7. The gate must explicitly hand off to `M256-E002`.

## Canonical models

- Evidence model:
  `a003-b004-c003-d004-summary-chain`
- Execution boundary model:
  `runnable-class-protocol-category-evidence-consumes-source-sema-lowering-and-runtime-proofs`
- Failure model:
  `fail-closed-on-class-protocol-category-conformance-evidence-drift`

## Non-goals

- No new parser, sema, lowering, or runtime feature work.
- No new object-model execution matrix yet; that belongs to `M256-E002`.
- No new metadata section families.
- No new property / ivar / storage realization work.

## Evidence

- `tmp/reports/m256/M256-E001/class_protocol_category_conformance_gate_summary.json`
