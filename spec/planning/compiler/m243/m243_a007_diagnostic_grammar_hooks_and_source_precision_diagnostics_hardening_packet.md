# M243-A007 - Diagnostic Grammar Hooks and Source Precision Diagnostics Hardening

## Metadata

- Packet: `M243-A007`
- Milestone: `M243`
- Lane: `A`
- Issue: `#6428`
- Dependencies: `M243-A006`

## Scope

Add parser diagnostic grammar-hook diagnostics hardening guardrails that are
explicitly deterministic and fail-closed before parse-recovery hardening and
cross-lane readiness advances.

## Required Code Anchors

- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Required Contract and Tooling Artifacts

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a007_expectations.md`
- `scripts/check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py`
- `tests/tooling/test_check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py`

## Validation Commands

- `python scripts/check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m243-a007-lane-a-readiness`

## Notes

- Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
- All temporary validation artifacts must be written under `tmp/` and retained.
