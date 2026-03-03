# M227-A010 Semantic Pass Conformance Corpus Expansion Packet

Packet: `M227-A010`
Milestone: `M227`
Lane: `A`
Dependencies: `M227-A009`

## Scope

Expand lane-A semantic-pass conformance corpus closure so parser/sema corpus accounting and replay-case continuity remain deterministic and fail-closed before lane-A integration closeout.

## Anchors

- Contract: `docs/contracts/m227_semantic_pass_conformance_corpus_expansion_expectations.md`
- Checker: `scripts/check_m227_a010_semantic_pass_conformance_corpus_expansion_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_a010_semantic_pass_conformance_corpus_expansion_contract.py`
- Sema contract surface: `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- Parser/sema handoff scaffold: `native/objc3c/src/sema/objc3_parser_sema_handoff_scaffold.h`
- Sema manager implementation: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`
- Lowering/runtime spec anchor: `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- Metadata/ABI spec anchor: `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Required Evidence

- `tmp/reports/m227/M227-A010/semantic_pass_conformance_corpus_expansion_summary.json`

## Determinism Criteria

- `Objc3ParserSemaConformanceCorpus` accounting (`required_case_count`, `passed_case_count`, `failed_case_count`) and replay-case flags remain explicit and deterministic.
- parser/sema handoff corpus synthesis keeps fail-closed readiness constraints (`required_case_count == 5`, `passed_case_count == required_case_count`, `failed_case_count == 0`).
- sema manager parity/result surfaces preserve corpus replay fail-closed continuity (`required_case_count > 0`, `failed_case_count == 0`).
- package lane readiness keeps `M227-A009` dependency continuity and A010 corpus-expansion checks synchronized.
