"""Release-claims source-surface result predicates."""

from __future__ import annotations

from collections.abc import Iterable

from ...case_result import CaseResult


def is_authoritative_case_result(
    result: CaseResult,
    case_ids: frozenset[str],
) -> bool:
    return result.case_id in case_ids


def authoritative_case_ids(
    results: Iterable[CaseResult],
    case_ids: frozenset[str],
) -> list[str]:
    return [
        result.case_id
        for result in results
        if is_authoritative_case_result(result, case_ids)
    ]


__all__ = [
    "authoritative_case_ids",
    "is_authoritative_case_result",
]
