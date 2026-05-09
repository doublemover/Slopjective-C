"""Shared helpers for storage/reflection lowering surface builders."""

from __future__ import annotations

from collections.abc import Container

from objc3c_runtime_acceptance.case_result import CaseResult


def authoritative_case_ids(
    results: list[CaseResult],
    accepted_case_ids: Container[str],
) -> list[str]:
    return [
        result.case_id
        for result in results
        if result.case_id in accepted_case_ids
    ]


__all__ = ["authoritative_case_ids"]
