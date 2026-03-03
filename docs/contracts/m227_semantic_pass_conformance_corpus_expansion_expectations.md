# M227 Semantic Pass Conformance Corpus Expansion Expectations (A010)

Contract ID: `objc3c-semantic-pass-conformance-corpus-expansion/m227-a010-v1`
Status: Accepted
Scope: lane-A semantic-pass conformance corpus expansion for parser/sema replay evidence continuity.
Dependencies: `M227-A009`

Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Objective

Expand conformance-corpus closure on top of A009 matrix implementation so required/passed/failed corpus accounting, replay-case presence, and deterministic corpus status remain explicit and fail-closed through handoff and sema manager surfaces.

## Deterministic Invariants

1. `Objc3ParserSemaConformanceCorpus` keeps explicit corpus accounting fields:
   - `required_case_count`
   - `passed_case_count`
   - `failed_case_count`
   - `has_recovery_replay_case`
   - `recovery_replay_case_passed`
2. Parser/sema handoff scaffold computes corpus counts deterministically and requires `required_case_count == 5`, `passed_case_count == required_case_count`, and `failed_case_count == 0` for deterministic corpus readiness.
3. Sema manager carries corpus surfaces to result/parity layers and preserves fail-closed replay guards (`required_case_count > 0`, `failed_case_count == 0`).
4. Lane-A readiness wiring keeps explicit A009 -> A010 dependency continuity.

## Validation

- `python scripts/check_m227_a010_semantic_pass_conformance_corpus_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a010_semantic_pass_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m227-a010-lane-a-readiness`

## Evidence Path

- `tmp/reports/m227/M227-A010/semantic_pass_conformance_corpus_expansion_summary.json`
