# M243-A006 - Diagnostic Grammar Hooks and Source Precision Edge-Case Expansion and Robustness

## Metadata

- Packet: `M243-A006`
- Milestone: `M243`
- Lane: `A`
- Issue: `#6427`
- Dependencies: `M243-A005`

## Scope

Expand parser diagnostic grammar-hook edge-case robustness so diagnostics UX and
fix-it engine continuity remains deterministic and fail-closed before parse
recovery, conformance, and downstream lowering-readiness decisions.

## Required Code Anchors

- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Required Contract and Tooling Artifacts

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a006_expectations.md`
- `scripts/check_m243_a006_diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract.py`
- `tests/tooling/test_check_m243_a006_diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract.py`

## Validation Commands

- `python scripts/check_m243_a006_diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a006_diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m243-a006-lane-a-readiness`

## Notes

- Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
- All temporary validation artifacts must be written under `tmp/` and retained.
