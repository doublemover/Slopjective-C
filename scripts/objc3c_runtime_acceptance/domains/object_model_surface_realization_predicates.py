"""Object Model realization acceptance helper predicates."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_result import CaseResult


def case_id_is_authoritative(case_id: str, accepted_case_ids: frozenset[str]) -> bool:
    return case_id in accepted_case_ids


def authoritative_case_ids(
    results: list[CaseResult],
    accepted_case_ids: frozenset[str],
) -> list[str]:
    return [
        result.case_id
        for result in results
        if case_id_is_authoritative(result.case_id, accepted_case_ids)
    ]


__all__ = ["authoritative_case_ids", "case_id_is_authoritative"]
