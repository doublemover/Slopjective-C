# M243-A002 Diagnostic Grammar Hooks and Source Precision Modular Split and Scaffolding Packet

Packet: `M243-A002`
Milestone: `M243`
Lane: `A`
Dependencies: `M243-A001`

## Scope

Execute modular split/scaffolding for lane-A parser diagnostic source precision
with deterministic parse-to-lowering handoff and fail-closed readiness anchors.

## Anchors

- Contract: `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a002_expectations.md`
- Checker: `scripts/check_m243_a002_diagnostic_grammar_hooks_and_source_precision_modular_split_scaffolding_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_a002_diagnostic_grammar_hooks_and_source_precision_modular_split_scaffolding_contract.py`
- Scaffold header: `native/objc3c/src/parse/objc3_diagnostic_source_precision_scaffold.h`
- Scaffold implementation: `native/objc3c/src/parse/objc3_diagnostic_source_precision_scaffold.cpp`
- Parse/lowering readiness integration: `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Surface type integration: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Native build graph wiring: `native/objc3c/CMakeLists.txt`
- Build script wiring: `scripts/build_objc3c_native.ps1`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m243/M243-A002/diagnostic_grammar_hooks_and_source_precision_modular_split_scaffolding_contract_summary.json`

## Determinism Criteria

- Parser diagnostics coordinate/code shape must be scaffolded into a stable key.
- Parse/lowering diagnostics hardening must fail closed when source-precision
  scaffold consistency drifts.
- Build graph declarations for parser scaffolding must remain explicit and
  deterministic across CMake and build script surfaces.
