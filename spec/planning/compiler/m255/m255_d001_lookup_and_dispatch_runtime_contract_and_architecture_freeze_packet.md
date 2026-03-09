# M255-D001 Lookup and Dispatch Runtime Contract and Architecture Freeze Packet

Packet: `M255-D001`
Milestone: `M255`
Lane: `D`
Issue: `#7123`
Contract ID: `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`

## Objective

Freeze the runtime-owned selector interning and dispatch boundary that the live
lane-C ABI now targets, so later selector-table, cache, and slow-path work
extends one preserved surface instead of redefining lookup/dispatch behavior ad
hoc.

## Scope Anchors

- Contract:
  `docs/contracts/m255_lookup_and_dispatch_runtime_contract_and_architecture_freeze_d001_expectations.md`
- Checker:
  `scripts/check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py`
- Tooling tests:
  `tests/tooling/test_check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py`
- Runtime probe:
  `tests/tooling/runtime/m255_d001_lookup_dispatch_runtime_probe.cpp`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m255-d001-lookup-and-dispatch-runtime-contract-and-architecture-freeze`
  - `test:tooling:m255-d001-lookup-and-dispatch-runtime-contract-and-architecture-freeze`
  - `check:objc3c:m255-d001-lane-d-readiness`
- Code anchors:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/parse/objc3_parser.cpp`
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `native/objc3c/src/runtime/README.md`
  - `tests/tooling/runtime/README.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Frozen Boundary

- Canonical selector lookup symbol: `objc3_runtime_lookup_selector`
- Canonical dispatch symbol: `objc3_runtime_dispatch_i32`
- Canonical selector handle type: `objc3_runtime_selector_handle`
- Selector interning model:
  `process-global-selector-intern-table-stable-id-per-canonical-selector-spelling`
- Lookup-table model:
  `metadata-backed-selector-lookup-tables-deferred-until-m255-d002`
- Cache model:
  `method-cache-and-runtime-slow-path-deferred-until-m255-d003`
- Protocol/category model:
  `protocol-and-category-aware-method-resolution-deferred-until-m255-d004`
- Compatibility model:
  `compatibility-shim-remains-test-only-non-authoritative-runtime-surface`
- Failure model:
  `runtime-lookup-and-dispatch-fail-closed-on-unmaterialized-resolution`

## Non-Goals

- no metadata-backed selector lookup tables yet
- no method-cache materialization yet
- no slow-path method lookup yet
- no protocol/category-aware runtime method resolution yet
- no widening of the live lane-C call ABI

## Acceptance Checklist

- one deterministic packet and expectations doc freeze the runtime-owned lookup
  and dispatch boundary
- canonical runtime/data-structure anchors remain explicit in lowering/runtime
  code and docs/spec
- the compatibility shim is explicitly documented as non-authoritative for the
  live lookup/dispatch runtime surface
- the runtime probe compiles against `native/objc3c/src/runtime/objc3_runtime.h`
  and `artifacts/lib/objc3_runtime.lib`
- the runtime probe proves selector interning stability, nil-dispatch behavior,
  dispatch formula parity, registration-state visibility, and reset replay of
  selector stable IDs
- evidence lands at
  `tmp/reports/m255/M255-D001/lookup_dispatch_runtime_contract_summary.json`

## Gate Commands

- `python scripts/check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m255-d001-lane-d-readiness`
