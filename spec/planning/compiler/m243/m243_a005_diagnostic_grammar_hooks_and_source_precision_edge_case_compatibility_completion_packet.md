# M243-A005 - Diagnostic Grammar Hooks and Source Precision Edge-Case Compatibility Completion

## Metadata

- Packet: `M243-A005`
- Milestone: `M243`
- Lane: `A`
- Issue: `#6426`
- Dependencies: `M243-A004`

## Scope

Complete parser diagnostic grammar-hook edge-case compatibility invariants so
compatibility handoff, pragma coordinate ordering, and parser token-budget
constraints are explicitly fail-closed before parse artifact edge robustness
and lowering-readiness decisions.

## Required Code Anchors

- `native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_edge_case_compatibility_surface.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Required Contract and Tooling Artifacts

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a005_expectations.md`
- `scripts/check_m243_a005_diagnostic_grammar_hooks_and_source_precision_edge_case_compatibility_completion_contract.py`
- `tests/tooling/test_check_m243_a005_diagnostic_grammar_hooks_and_source_precision_edge_case_compatibility_completion_contract.py`

## Validation Commands

- `python scripts/check_m243_a005_diagnostic_grammar_hooks_and_source_precision_edge_case_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a005_diagnostic_grammar_hooks_and_source_precision_edge_case_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m243-a005-lane-a-readiness`

## Notes

- Edge-case compatibility completion must stay deterministic and fail closed.
- Any temporary validation artifacts must be written under `tmp/` and retained.
