# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A008)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-recovery-and-determinism-hardening/m243-a008-v1`
Status: Accepted
Scope: parser diagnostic grammar-hook recovery and determinism hardening with deterministic fail-closed readiness progression.

## Objective

Expand A007 diagnostics hardening closure so parser diagnostic grammar-hook
surfaces preserve deterministic recovery/determinism consistency and readiness
before conformance matrix and downstream lane-E readiness advances.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-A007`
- A007 diagnostics hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a007_expectations.md`
  - `spec/planning/compiler/m243/m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_packet.md`
  - `scripts/check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py`

## Deterministic Invariants

1. Parse/lowering readiness carries parser diagnostic grammar-hook recovery and determinism fields:
   - `parser_diagnostic_grammar_hooks_recovery_determinism_consistent`
   - `parser_diagnostic_grammar_hooks_recovery_determinism_ready`
   - `parser_diagnostic_grammar_hooks_recovery_determinism_key`
2. `pipeline/objc3_parse_lowering_readiness_surface.h` computes recovery and
   determinism readiness deterministically from parse recovery hardening,
   parser diagnostic hardening readiness, parser replay readiness, and replay-key continuity.
3. Fail-closed failure reasons explicitly cover parser diagnostic grammar-hook
   recovery and determinism hardening drift.
4. Frontend artifact JSON projects parser diagnostic grammar-hook recovery and
   determinism booleans and replay key.
5. A007 dependency continuity is explicit in docs and packet anchors.

## Architecture and Build Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-A A008
  recovery and determinism hardening anchor text.
- `package.json` includes:
  - `check:objc3c:m243-a008-diagnostic-grammar-hooks-and-source-precision-recovery-and-determinism-hardening-contract`
  - `test:tooling:m243-a008-diagnostic-grammar-hooks-and-source-precision-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m243-a008-lane-a-readiness`

## Validation

- `python scripts/check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m243-a008-lane-a-readiness`

## Evidence Path

- `tmp/reports/m243/M243-A008/diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract_summary.json`

