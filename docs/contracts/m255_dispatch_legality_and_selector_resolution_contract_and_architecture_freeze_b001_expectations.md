# M255 Dispatch Legality and Selector Resolution Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-dispatch-legality-selector-resolution/m255-b001-v1`

## Objective

Freeze the legality and selector-resolution boundary that the live dispatch path
consumes so later semantic/runtime implementation issues preserve one explicit,
fail-closed rule set.

## Required implementation

1. Add a canonical expectations document for the freeze.
2. Add this packet, a deterministic checker, tooling tests, and a lane-B
   readiness runner:
   - `scripts/check_m255_b001_dispatch_legality_and_selector_resolution_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m255_b001_dispatch_legality_and_selector_resolution_contract_and_architecture_freeze.py`
   - `scripts/run_m255_b001_lane_b_readiness.py`
3. Add `M255-B001` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
4. Freeze the live legality boundary explicitly:
   - receiver must remain explicit
   - selector forms remain normalized unary/keyword selectors only
   - selector ambiguity or unresolved resolution remains fail-closed
   - overload-style recovery remains a non-goal
   - direct dispatch remains reserved
5. `package.json` must wire:
   - `check:objc3c:m255-b001-dispatch-legality-and-selector-resolution-contract-and-architecture-freeze`
   - `test:tooling:m255-b001-dispatch-legality-and-selector-resolution-contract-and-architecture-freeze`
   - `check:objc3c:m255-b001-lane-b-readiness`
6. The freeze must explicitly hand off to `M255-B002`.

## Non-goals

- No new selector-resolution implementation.
- No overload recovery.
- No direct-dispatch enablement.
- No runtime ABI expansion.

## Evidence

- `tmp/reports/m255/M255-B001/dispatch_legality_selector_resolution_contract_summary.json`
