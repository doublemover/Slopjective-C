"""Quality-gate rollup decision logic."""

from __future__ import annotations


def determine_qg04_result(
    gate_results: list[dict[str, str]], active_exception_ids: list[str]
) -> str:
    statuses = {item["status"] for item in gate_results}
    if "blocked" in statuses:
        return "blocked"
    if "fail" in statuses:
        return "fail"
    if statuses == {"pass"}:
        return "conditional-pass" if active_exception_ids else "pass"
    return "conditional-pass"


def recommendation_signal_for(qg04_result: str) -> str:
    return {
        "pass": "go-candidate",
        "conditional-pass": "conditional-go-candidate",
        "fail": "no-go",
        "blocked": "hold",
    }[qg04_result]


def determine_decision(qg04_result: str) -> str:
    return "approve" if qg04_result == "pass" else "hold"


__all__ = [
    "determine_decision",
    "determine_qg04_result",
    "recommendation_signal_for",
]
