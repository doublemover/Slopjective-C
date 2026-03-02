# objc3c Native Frontend Architecture Contract

Status: Accepted (M132-A001)
Scope: `native/objc3c/src/*`

This document defines dependency rules for the modular native frontend.

## Layer Model

The frontend is split into five layers plus integration adapters:

1. `lex/*`
2. `parse/*`
3. `sema/*`
4. `lower/*`
5. `ir/*`

Adapter and integration modules:

- `pipeline/*`: stage orchestration, contracts, stage result passing.
- `libobjc3c_frontend/*`: public embedding API over `pipeline/*`.
- `driver/*`: CLI argument parsing and exit-code mapping only.
- `io/*`: filesystem/process adapters for manifests, artifacts, and tool calls.

Current-state note:

- `main.cpp` is now driver-only and delegates to `driver/objc3_driver_main.h`.
- Parser/sema boundaries are enforced through explicit contracts in
  `parse/objc3_parser_contract.h`, `parse/objc3_ast_builder_contract.h`, and
  `sema/objc3_sema_contract.h`.
- M226 architecture-freeze work builds on this extracted layout and hardens
  parser completeness and parser-to-sema handoff determinism.
- M227 extends the sema boundary with pass-order and symbol-flow freeze rules
  in `sema/objc3_sema_pass_manager_contract.h` (`Objc3SemaPassFlowSummary`).
- M227 lane-B type-system freeze anchors canonical ObjC form sets in
  `sema/objc3_sema_contract.h` (`kObjc3CanonicalReferenceTypeForms`,
  `kObjc3CanonicalScalarMessageSendTypeForms`,
  `kObjc3CanonicalBridgeTopReferenceTypeForms`) to keep semantic checking
  deterministic across `id`/`Class`/`SEL`/`Protocol`/`instancetype` and
  object-pointer forms.
- M250 lane-A frontend stability freeze anchors long-tail grammar closure to
  parser contract snapshots (`parse/objc3_parser_contract.h`) and parse/lowering
  readiness replay gates (`pipeline/objc3_parse_lowering_readiness_surface.h`)
  so parser determinism and recovery coverage remain fail-closed for GA
  readiness.
- M250 lane-B semantic stability freeze closes spec delta between
  `pipeline/objc3_typed_sema_to_lowering_contract_surface.h` and
  `pipeline/objc3_parse_lowering_readiness_surface.h` so semantic handoff
  determinism, conformance corpus closure, and performance guardrail gating are
  fail-closed under a single readiness boundary.

## Ownership Map

- Lane A: `lex/*`, `parse/*`, `lower/*`, `ir/*`, `driver/*`, `io/*`
- Lane B: `pipeline/*`, `sema/*`, `libobjc3c_frontend/*`
- Lane C: generated docs/site tooling and policies (outside `src` tree)
- Lane D: parity/perf/determinism/fuzz gates (tests + scripts)
- Lane E: workflow/governance docs and CI policy wiring

## Dependency Rules

Rules apply to both include direction and link direction.

Allowed dependencies:

- `driver -> libobjc3c_frontend, io`
- `libobjc3c_frontend -> pipeline`
- `pipeline -> lex, parse, sema, lower, ir`
- `lower -> sema`
- `ir -> lower`
- `io -> (no frontend layer dependencies)`
- `lex -> (none)`
- `parse -> lex`
- `sema -> parse`

Forbidden dependencies:

- No stage module may depend on `driver/*`.
- No stage module may depend on `io/*`.
- `lex/*` may not depend on `parse/*`, `sema/*`, `lower/*`, `ir/*`.
- `parse/*` may not depend on `sema/*`, `lower/*`, `ir/*`.
- `sema/*` may not depend on `lower/*`, `ir/*`.
- `lower/*` may not depend on `ir/*`.
- `pipeline/*` may not depend on `driver/*` or `io/*`.
- `libobjc3c_frontend/*` may not depend on `driver/*` or `io/*`.

## Contract Checks (Automation Targets)

The future boundary-check script (`scripts/check_objc3c_dependency_boundaries.py`)
shall enforce:

- Include graph constraints for `#include "..."` within `native/objc3c/src`.
- CMake target link-direction constraints aligned with this document.
- A denylist for reverse dependencies violating stage order.

## Examples

Allowed:

- `parse/objc3_parser.cpp` including `lex/objc3_lexer.h`
- `pipeline/frontend_pipeline.cpp` including `sema/semantic_passes.h`
- `driver/main_driver.cpp` including `libobjc3c_frontend/api.h`

Forbidden:

- `sema/semantic_passes.cpp` including `lower/lower_to_ir.h`
- `ir/ir_emitter.cpp` including `driver/cli_options.h`
- `lex/objc3_lexer.cpp` including `parse/objc3_parser.h`

## Refactor Safety Invariants

- Behavior parity is enforced outside this file by Lane D gates.
- New modules must keep deterministic diagnostics ordering.
- `main.cpp` remains orchestration-only and must not absorb parser/sema logic.
- Parser recovery behavior must remain replay-proof and deterministic.
