# M253-E001 Metadata Emission Gate Contract And Architecture Freeze Packet

Packet: `M253-E001`
Milestone: `M253`
Lane: `E`
Issue: `#7099`

## Objective

Freeze the aggregate metadata emission gate so later closeout work must preserve one canonical evidence contract proving that matrix coverage, object-format policy, binary inspection, and archive/static-link discovery are all live and synchronized.

## Dependencies

- none

## Required implementation

1. Add a canonical expectations document for the metadata emission gate.
2. Add this packet, a deterministic checker, and tooling tests:
   - `scripts/check_m253_e001_metadata_emission_gate.py`
   - `tests/tooling/test_check_m253_e001_metadata_emission_gate.py`
3. Add `M253-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/io/objc3_process.cpp`
4. Keep the gate fail closed over the completed upstream evidence:
   - `M253-A002`
   - `M253-B003`
   - `M253-C006`
   - `M253-D003`
5. Require the checker to validate the upstream summary files directly and reject drift when any required payload stops reporting `ok: true`, loses its probe execution flag, or drops the case-level invariants that define metadata emission readiness.
6. Require the checker to prove the aggregate gate preserves all pre-closeout prerequisites:
   - A002 source-to-section matrix continuity,
   - B003 host object-format policy continuity,
   - C006 binary inspection corpus continuity,
   - D003 archive/static-link discovery continuity.
7. Wire `package.json` so lane-E readiness explicitly chains:
   - `check:objc3c:m253-a002-lane-a-readiness`
   - `check:objc3c:m253-b003-lane-b-readiness`
   - `check:objc3c:m253-c006-lane-c-readiness`
   - `check:objc3c:m253-d003-lane-d-readiness`
   - `check:objc3c:m253-e001-metadata-emission-gate`
   - `test:tooling:m253-e001-metadata-emission-gate`
   - `check:objc3c:m253-e001-lane-e-readiness`
8. Make the packet explicit that the next implementation issue `M253-E002` must preserve this aggregate gate while landing cross-lane closeout.

## Determinism and fail-closed rules

- `M253-E001` is an aggregate evidence freeze; it does not add new metadata section emission behavior or runtime registration.
- If any upstream evidence file disappears or drifts, the gate must fail closed.

## Evidence

- `tmp/reports/m253/M253-E001/metadata_emission_gate_summary.json`
