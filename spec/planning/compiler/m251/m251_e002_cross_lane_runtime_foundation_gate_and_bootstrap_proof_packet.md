# M251-E002 Cross-Lane Runtime-Foundation Gate and Bootstrap Proof Packet

Packet: `M251-E002`
Milestone: `M251`
Lane: `E`
Issue: `#7069`

## Objective

Add the first real integrated runtime-foundation gate that fails closed unless
source-record handoff, semantic diagnostics, metadata-section/object inspection,
and runtime-library execution all line up on the same native toolchain path.

## Dependencies

- `M251-A003`
- `M251-B003`
- `M251-C003`
- `M251-D003`
- `M251-E001`

## Required implementation

1. Add a canonical lane-E expectations document for the integrated gate.
2. Add this packet, a deterministic checker, and tooling tests for the gate:
   - `scripts/check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`
   - `tests/tooling/test_check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`
3. Add lane-E E002 anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Require the checker to load the canonical `M251-E001` summary and fail
   closed if it is missing or no longer reports `ok: true`.
5. Require the checker to run a metadata-rich native compile probe against
   `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   and verify that:
   - `module.manifest.json` exists
   - `runtime_metadata_source_records` remain published
   - `runtime_export_ready_for_runtime_export` remains true on the integrated path
6. Require the checker to run an incomplete runtime-export probe against
   `tests/tooling/fixtures/native/m251_runtime_export_diagnostics_incomplete_interface.objc3`
   and verify precise `O3S260` diagnostics.
7. Require the checker to run a zero-descriptor object-emission probe against
   `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3`
   and verify runtime metadata sections/symbols with:
   - `llvm-readobj --sections`
   - `llvm-objdump --syms`
8. Require the checker to run a fresh execution-smoke replay under run id
   `m251_e002_cross_lane_runtime_foundation_gate` and fail closed unless:
   - `status = PASS`
   - `runtime_library = artifacts/lib/objc3_runtime.lib`
   - at least one passing positive runtime-linked fixture is observed
9. Wire `package.json` so lane-E readiness explicitly chains:
   - `check:objc3c:m251-e001-lane-e-readiness`
   - `check:objc3c:m251-e002-cross-lane-runtime-foundation-gate-and-bootstrap-proof`
   - `test:tooling:m251-e002-cross-lane-runtime-foundation-gate-and-bootstrap-proof`

## Determinism and fail-closed rules

- `M251-E002` is an integrated gate only; it does not land new runtime
  features.
- The packet therefore fails closed when any dependency drift, missing tool,
  manifest gap, diagnostic regression, object-inspection regression, or smoke
  replay regression is observed.
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicit test-only
  evidence and must not reappear as the canonical runtime library.

## Validation plan

The checker runs four real integrated probes:

1. Metadata-rich native compile with manifest preservation checks.
2. Precise incomplete-runtime-export diagnostic probe.
3. Zero-descriptor object inspection probe using emitted objects.
4. Fresh runtime-linked execution-smoke replay.

## Evidence

- `tmp/reports/m251/M251-E002/cross_lane_runtime_foundation_gate_summary.json`
