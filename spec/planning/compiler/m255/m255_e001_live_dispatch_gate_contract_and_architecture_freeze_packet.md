# M255-E001 Live Dispatch Gate Contract And Architecture Freeze Packet

Packet: `M255-E001`
Milestone: `M255`
Lane: `E`
Dependencies: `M255-A002`, `M255-B003`, `M255-C004`, `M255-D004`

## Purpose

Freeze the fail-closed lane-E evidence contract proving supported message sends
execute through the live runtime path rather than the compatibility shim, while
handing off smoke/closeout replacement to `M255-E002`.

## Scope anchors

- Contract:
  `docs/contracts/m255_live_dispatch_gate_contract_and_architecture_freeze_e001_expectations.md`
- Checker:
  `scripts/check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py`
- Tooling tests:
  `tests/tooling/test_check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py`
- Lane readiness:
  `scripts/run_m255_e001_lane_e_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m255-e001-live-dispatch-gate`
  - `test:tooling:m255-e001-live-dispatch-gate`
  - `check:objc3c:m255-e001-lane-e-readiness`

## Gate boundary

- Contract id `objc3c-runtime-live-dispatch-gate/m255-e001-v1`
- Evidence model `a002-b003-c004-d004-summary-chain`
- Shim boundary model
  `live-runtime-dispatch-required-compatibility-shim-evidence-only`
- Failure model `fail-closed-on-live-dispatch-evidence-drift`

## Acceptance checklist

- the gate freezes explicit non-goals and fail-closed behavior
- the gate requires `M255-C004` to keep supported live sends on
  `objc3_runtime_dispatch_i32`
- the gate requires `M255-D004` to keep live category-backed method resolution
  and protocol-backed negative lookup evidence on the runtime path
- the gate preserves `M255-A002` and `M255-B003` as the taxonomy/legality
  continuity inputs for the next integrated closeout issue
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains exported only as
  compatibility/test evidence and is not treated as authoritative live-dispatch
  proof
- validation evidence lands at
  `tmp/reports/m255/M255-E001/live_dispatch_gate_summary.json`
- `M255-E002` is the explicit next handoff for replacing shim-based smoke and
  closeout assumptions
