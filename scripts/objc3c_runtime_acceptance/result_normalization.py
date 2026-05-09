"""Runtime acceptance result normalization."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult


def normalize_case_result(result: CaseResult) -> dict[str, Any]:
    return {
        "case_id": result.case_id,
        "probe": result.probe,
        "fixture": result.fixture,
        "claim_class": result.claim_class,
        "passed": result.passed,
        "summary": result.summary,
    }


def normalize_case_results(results: list[CaseResult]) -> list[dict[str, Any]]:
    return [normalize_case_result(result) for result in results]


__all__ = ["normalize_case_result", "normalize_case_results"]
