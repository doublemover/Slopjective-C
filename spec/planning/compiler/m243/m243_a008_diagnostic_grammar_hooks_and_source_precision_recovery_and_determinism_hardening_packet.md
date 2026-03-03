# M243-A008 Diagnostic Grammar Hooks and Source Precision Recovery and Determinism Hardening Packet

Packet: `M243-A008`
Milestone: `M243`
Lane: `A`
Freeze date: `2026-03-03`
Dependencies: `M243-A007`

## Purpose

Freeze lane-A parser diagnostic grammar-hook recovery and determinism hardening
prerequisites so diagnostics hardening and replay continuity remain deterministic
and fail-closed before conformance matrix and lane-E gates advance.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Required Code Anchors

- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Required Contract and Tooling Artifacts

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a008_expectations.md`
- `scripts/check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py`
- `tests/tooling/test_check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py`

## Dependency Anchors (M243-A007)

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a007_expectations.md`
- `spec/planning/compiler/m243/m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_packet.md`
- `scripts/check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py`
- `tests/tooling/test_check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py`

## Validation Commands

- `python scripts/check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m243-a008-lane-a-readiness`

## Evidence Output

- `tmp/reports/m243/M243-A008/diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract_summary.json`

## Notes

- All temporary validation artifacts must be written under `tmp/` and retained.

