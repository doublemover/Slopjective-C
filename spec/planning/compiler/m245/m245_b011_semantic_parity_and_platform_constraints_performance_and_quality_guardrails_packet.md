# M245-B011 Semantic Parity and Platform Constraints Performance and Quality Guardrails Packet

Packet: `M245-B011`
Milestone: `M245`
Lane: `B`
Theme: `performance and quality guardrails`
Issue: `#6633`
Freeze date: `2026-03-04`
Dependencies: `M245-B010`

## Purpose

Freeze lane-B performance and quality guardrails prerequisites for M245 semantic parity and platform constraints so dependency continuity stays explicit, deterministic, and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m245_semantic_parity_and_platform_constraints_performance_and_quality_guardrails_b011_expectations.md`
- Checker:
  `scripts/check_m245_b011_semantic_parity_and_platform_constraints_performance_and_quality_guardrails_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_b011_semantic_parity_and_platform_constraints_performance_and_quality_guardrails_contract.py`
- Dependency anchors from `M245-B010`:
  - `docs/contracts/m245_semantic_parity_and_platform_constraints_conformance_corpus_expansion_b010_expectations.md`
  - `spec/planning/compiler/m245/m245_b010_semantic_parity_and_platform_constraints_conformance_corpus_expansion_packet.md`
  - `scripts/check_m245_b010_semantic_parity_and_platform_constraints_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m245_b010_semantic_parity_and_platform_constraints_conformance_corpus_expansion_contract.py`

## Gate Commands

- `python scripts/check_m245_b011_semantic_parity_and_platform_constraints_performance_and_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m245_b011_semantic_parity_and_platform_constraints_performance_and_quality_guardrails_contract.py -q`

## Evidence Output

- `tmp/reports/m245/M245-B011/semantic_parity_and_platform_constraints_performance_and_quality_guardrails_summary.json`
