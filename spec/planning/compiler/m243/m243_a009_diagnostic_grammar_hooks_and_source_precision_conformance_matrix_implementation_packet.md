# M243-A009 Diagnostic Grammar Hooks and Source Precision Conformance Matrix Implementation Packet

Packet: `M243-A009`
Milestone: `M243`
Lane: `A`
Freeze date: `2026-03-03`
Dependencies: `M243-A008`

## Purpose

Freeze lane-A parser diagnostic grammar-hook conformance matrix implementation
prerequisites so conformance matrix consistency/readiness and deterministic key
continuity remain fail-closed before parse/lowering conformance corpus and
performance-quality gates advance.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Required Code Anchors

- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Required Contract and Tooling Artifacts

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a009_expectations.md`
- `scripts/check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py`
- `tests/tooling/test_check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py`

## Dependency Anchors (M243-A008)

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a008_expectations.md`
- `spec/planning/compiler/m243/m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_packet.md`
- `scripts/check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py`
- `tests/tooling/test_check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py`

## Validation Commands

- `python scripts/check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m243-a009-lane-a-readiness`

## Evidence Output

- `tmp/reports/m243/M243-A009/diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract_summary.json`

## Notes

- All temporary validation artifacts must be written under `tmp/` and retained.

