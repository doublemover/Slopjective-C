# M255-B001 Dispatch Legality and Selector Resolution Contract And Architecture Freeze Packet

Packet: `M255-B001`
Milestone: `M255`
Lane: `B`
Issue: `M255-B001`
Contract ID: `objc3c-dispatch-legality-selector-resolution/m255-b001-v1`

## Scope

Freeze the selector-resolution and dispatch-legality boundary that the current
native dispatch path consumes before semantic/runtime implementation issues widen
behavior.

## Required outputs

- `scripts/check_m255_b001_dispatch_legality_and_selector_resolution_contract_and_architecture_freeze.py`
- `tests/tooling/test_check_m255_b001_dispatch_legality_and_selector_resolution_contract_and_architecture_freeze.py`
- `scripts/run_m255_b001_lane_b_readiness.py`
- `tmp/reports/m255/M255-B001/dispatch_legality_selector_resolution_contract_summary.json`

## Frozen legality model

- receiver-required
- unary-and-keyword-selectors only
- fail-closed-on-unresolved-or-ambiguous-selector-resolution
- no overload recovery
- direct dispatch reserved

## Canonical gate command

- `check:objc3c:m255-b001-lane-b-readiness`
