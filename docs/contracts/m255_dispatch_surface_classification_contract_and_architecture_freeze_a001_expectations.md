# M255 Dispatch Surface Classification Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-dispatch-surface-classification/m255-a001-v1`

## Objective

Freeze the dispatch-surface classification boundary for the live runtime path so
instance, class, super, direct, and dynamic dispatch sites are classified
explicitly before `M255-A002` expands the implementation.

## Required implementation

1. Add a canonical expectations document for the freeze.
2. Add this packet, a deterministic checker, and tooling tests:
   - `scripts/check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py`
   - `scripts/run_m255_a001_lane_a_readiness.py`
3. Add `M255-A001` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
4. Freeze the current live bindings explicitly:
   - instance dispatch -> live runtime dispatch family
   - class dispatch -> live runtime dispatch family
   - super dispatch -> live runtime dispatch family
   - dynamic dispatch -> live runtime dispatch family
   - direct dispatch -> reserved non-goal in `M255-A001`
5. `package.json` must wire:
   - `check:objc3c:m255-a001-dispatch-surface-classification-contract-and-architecture-freeze`
   - `test:tooling:m255-a001-dispatch-surface-classification-contract-and-architecture-freeze`
   - `check:objc3c:m255-a001-lane-a-readiness`
6. The freeze must explicitly hand off to `M255-A002`.

## Non-goals

- No new dispatch entrypoint families.
- No new super/class/direct runtime lowering.
- No new parser syntax.
- No runtime ABI expansion beyond the frozen classification contract.

## Evidence

- `tmp/reports/m255/M255-A001/dispatch_surface_classification_contract_summary.json`
