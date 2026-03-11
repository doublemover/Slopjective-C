# M261-D001 Block Runtime API And Object Layout Contract And Architecture Freeze Packet

Packet: `M261-D001`
Milestone: `M261`
Wave: `W53`
Lane: `D`
Issue: `#7189`
Contract ID: `objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1`
Dependencies: none

## Objective

Freeze the truthful private runtime API and object-layout boundary for block
promotion, invoke hooks, and runtime-owned block records after `M261-C004`.

## Canonical Scope

- contract id `objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1`
- current public surface `stable-public-runtime-header-excludes-block-helper-entrypoints`
- current private helper surface `objc3_runtime_promote_block_i32-and-objc3_runtime_invoke_block_i32-remain-private-to-objc3_runtime_bootstrap_internal_h`
- current handle model `i32`
- current object-layout model `runtime-block-records-are-private-runtime-state-not-public-object-abi`
- current fail-closed model `byref-forwarding-and-owned-capture-escaping-block-lifetimes-remain-deferred-until-m261-d002-and-m261-d003`

## Acceptance Criteria

- Freeze the boundary for block runtime API and object layout with explicit
  non-goals and fail-closed rules.
- Document the canonical anchors the next implementation issue must preserve.
- Code/spec anchors remain explicit and deterministic.
- Validation evidence lands under `tmp/` with a stable path for the issue.

## Dynamic Probes

1. Positive freeze proof:
   - fixture `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_argument_positive.objc3`
   - expected run exit `14`
   - IR must carry
     `; runtime_block_api_object_layout = ...`,
     `@objc3_runtime_promote_block_i32`, and
     `@objc3_runtime_invoke_block_i32`
   - object backend must remain `llvm-direct`

## Non-Goals

- public block-object ABI
- generalized block allocation/copy/dispose runtime API
- byref-forwarded escaping block realization
- owned-object escaping block lifetime realization

## Validation Commands

- `python scripts/check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m261-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m261/M261-D001/block_runtime_api_object_layout_contract_summary.json`
- `M261-D002` is the explicit next issue after this freeze lands.
