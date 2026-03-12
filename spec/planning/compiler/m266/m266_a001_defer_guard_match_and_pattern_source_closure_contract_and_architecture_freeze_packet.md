# M266-A001 Packet: Defer, Guard, Match, And Pattern Source Closure Contract And Architecture Freeze

## Objective

Publish one truthful lane-A packet for the current Part 5 frontend boundary.

## Scope

- admit and count the existing parser-owned `guard` binding surface
- admit and count the existing `switch` / `case` pattern carrier
- reserve `defer` and `match` as explicit keywords
- fail closed on `defer` / `match` with targeted parser diagnostics
- emit one manifest summary under `frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure`

## Non-goals

- runnable `defer` lowering
- runnable `match` lowering
- new pattern forms beyond the existing `switch` / `case` carrier
- semantic or lowering work that belongs to later `M266` issues

## Truth boundary

- `guard` remains live parser-owned syntax
- `switch` / `case` remains the only supported pattern carrier today
- `defer` and `match` are reserved, parser-owned, and fail closed
- this issue freezes the frontend/source-model truth only

## Required anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CROSS_CUTTING_RULE_INDEX.md`
- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `package.json`

## Validation

- issue-local checker
- issue-local pytest wrapper
- fast frontend runner probe over one positive row and two fail-closed negative rows
- evidence written to `tmp/reports/m266/M266-A001/control_flow_source_closure_summary.json`
