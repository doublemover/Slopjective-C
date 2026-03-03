# M243-A003 Diagnostic Grammar Hooks and Source Precision Core Feature Implementation Packet

Packet: `M243-A003`
Milestone: `M243`
Lane: `A`
Dependencies: `M243-A002`

## Scope

Implement core parser diagnostic grammar-hook and source-precision feature
surfaces with fail-closed parse-to-lowering readiness integration.

## Anchors

- Contract: `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a003_expectations.md`
- Checker: `scripts/check_m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract.py`
- Core feature header: `native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_core_feature.h`
- Core feature implementation: `native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp`
- Source-precision scaffold dependency: `native/objc3c/src/parse/objc3_diagnostic_source_precision_scaffold.cpp`
- Parse/lowering integration: `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Surface type integration: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Build graph wiring: `native/objc3c/CMakeLists.txt`
- Build script wiring: `scripts/build_objc3c_native.ps1`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m243/M243-A003/diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract_summary.json`

## Determinism Criteria

- Parser diagnostics must preserve grammar-hook code namespace (`O3P###`) and
  source-coordinate ordering through the core feature surface.
- Parse/lowering diagnostics hardening must fail closed if grammar-hook core
  feature consistency drifts.
- Build topology for parser grammar-hook core feature remains explicit and
  deterministic across native build entrypoints.
