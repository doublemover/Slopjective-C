# M243-A004 - Diagnostic Grammar Hooks and Source Precision Core Feature Expansion

## Metadata

- Packet: `M243-A004`
- Milestone: `M243`
- Lane: `A`
- Issue: `#6425`
- Dependencies: `M243-A003`

## Scope

Expand parser diagnostic grammar-hook/source-precision readiness from
core-feature implementation to core-feature expansion by adding explicit
case-accounting and replay-key guardrails that fail closed before lowering
readiness can advance.

## Required Code Anchors

- `native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Required Contract and Tooling Artifacts

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a004_expectations.md`
- `scripts/check_m243_a004_diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract.py`
- `tests/tooling/test_check_m243_a004_diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract.py`

## Validation Commands

- `python scripts/check_m243_a004_diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a004_diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m243-a004-lane-a-readiness`

## Notes

- Expansion logic must remain deterministic and fail closed.
- Any temporary validation artifacts must be written under `tmp/` and retained.
