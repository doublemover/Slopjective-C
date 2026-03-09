# M255 Live Dispatch Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-runtime-live-dispatch-gate/m255-e001-v1`

## Objective

Freeze one fail-closed lane-E evidence gate proving supported message sends
execute through the live runtime path rather than the compatibility shim.

## Required implementation

1. Add a canonical expectations document for the live dispatch gate.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-E
   readiness runner:
   - `scripts/check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py`
   - `scripts/run_m255_e001_lane_e_readiness.py`
3. Add `M255-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
4. Keep the gate fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m255/M255-A002/dispatch_site_modeling_summary.json`
   - `tmp/reports/m255/M255-B003/super_direct_dynamic_method_family_summary.json`
   - `tmp/reports/m255/M255-C004/live_dispatch_cutover_summary.json`
   - `tmp/reports/m255/M255-D004/protocol_category_method_resolution_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops
   reporting `ok: true`, or drops the live-path invariants that define the
   current runtime-dispatch boundary.
6. `package.json` must wire:
   - `check:objc3c:m255-e001-live-dispatch-gate`
   - `test:tooling:m255-e001-live-dispatch-gate`
   - `check:objc3c:m255-e001-lane-e-readiness`
7. The gate must explicitly hand off to `M255-E002`.

## Canonical models

- Evidence model:
  `a002-b003-c004-d004-summary-chain`
- Shim boundary model:
  `live-runtime-dispatch-required-compatibility-shim-evidence-only`
- Failure model:
  `fail-closed-on-live-dispatch-evidence-drift`

## Non-goals

- No new dispatch lowering implementation.
- No new runtime lookup or category/protocol resolution feature work.
- No removal of the exported `objc3_msgsend_i32` compatibility symbol.
- No smoke/closeout rewrite yet; that belongs to `M255-E002`.

## Evidence

- `tmp/reports/m255/M255-E001/live_dispatch_gate_summary.json`
