# M251-E001 Runnable-Runtime Foundation Gate and Evidence Contract Packet

Packet: `M251-E001`
Milestone: `M251`
Lane: `E`
Issue: `#7068`

## Objective

Freeze the aggregate M251 runtime-foundation gate so future runtime milestones
must preserve one canonical evidence contract proving that runtime metadata
handoff, diagnostics, object inspection, and emitted-object runtime linkage are
all live and synchronized.

## Dependencies

- none

## Required implementation

1. Add a canonical lane-E expectations document for the runnable-runtime
   foundation gate.
2. Add this packet, a deterministic checker, and tooling tests for the gate:
   - `scripts/check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py`
   - `tests/tooling/test_check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py`
3. Add lane-E M251 anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Keep the gate fail closed over the completed upstream evidence:
   - `M251-A003`
   - `M251-B003`
   - `M251-C003`
   - `M251-D003`
5. Require the checker to validate the upstream summary files directly and
   reject drift when any required payload stops reporting `ok: true` or drops
   the required dynamic proof rows.
6. Require the checker to validate the D003 execution-smoke summary directly at
   `tmp/artifacts/objc3c-native/execution-smoke/m251_d003_runtime_library_link_wiring/summary.json`
   and reject drift when:
   - `status != PASS`
   - `runtime_library != artifacts/lib/objc3_runtime.lib`
   - runtime-linked positive fixtures stop consuming the runtime archive
7. Require the checker to validate the native runtime archive path
   `artifacts/lib/objc3_runtime.lib` exists when the gate is executed.
8. Wire `package.json` so lane-E readiness explicitly chains:
   - `check:objc3c:m251-a003-lane-a-readiness`
   - `check:objc3c:m251-b003-lane-b-readiness`
   - `check:objc3c:m251-c003-lane-c-readiness`
   - `check:objc3c:m251-d003-lane-d-readiness`
   - `check:objc3c:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract`
   - `test:tooling:m251-e001-runnable-runtime-foundation-gate-and-evidence-contract`

## Determinism and fail-closed rules

- `M251-E001` is an aggregate evidence freeze; it does not land new runtime
  registration, class realization, property execution, blocks, ARC, or module
  support.
- The packet therefore treats the completed A003/B003/C003/D003 artifacts as
  the canonical foundation proof for the next milestone.
- If any upstream evidence file disappears, drifts, or stops reporting its
  required dynamic probes, the lane-E gate must fail closed.
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains test-only evidence
  and must not reappear as the canonical runtime library.

## Validation plan

The checker performs deterministic aggregate validation over already-materialized
runtime-foundation evidence:

1. Static snippet validation across expectations/packet/docs/specs/package.
2. JSON summary validation over the canonical A003/B003/C003/D003 summary
   files.
3. D003 execution-smoke summary validation proving the native runtime archive
   is the canonical runtime-linked execution target.
4. Native archive existence validation for `artifacts/lib/objc3_runtime.lib`.

## Evidence

- `tmp/reports/m251/M251-E001/runnable_runtime_foundation_gate_contract_summary.json`
