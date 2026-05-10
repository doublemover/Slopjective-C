"""Acceptance-report selection helpers for runnable interop conformance."""

from __future__ import annotations

from typing import Any

from .constants import REQUIRED_CASES, REQUIRED_SURFACE_CONTRACTS


def _case_map(acceptance_report: dict[str, Any] | None) -> dict[str, dict[str, Any]]:
    if not isinstance(acceptance_report, dict):
        return {}

    cases = acceptance_report.get("cases", [])
    if not isinstance(cases, list):
        return {}

    return {
        str(case.get("case_id")): case
        for case in cases
        if isinstance(case, dict) and case.get("case_id") is not None
    }


def report_has_required_inputs(acceptance_report: dict[str, Any] | None) -> bool:
    case_map = _case_map(acceptance_report)
    report_has_required_cases = bool(case_map) and all(
        case_map.get(case_id, {}).get("passed") is True for case_id in REQUIRED_CASES
    )
    report_has_required_surfaces = isinstance(acceptance_report, dict) and all(
        isinstance(acceptance_report.get(surface_key), dict)
        for surface_key in REQUIRED_SURFACE_CONTRACTS
    )
    return report_has_required_cases and report_has_required_surfaces


def load_acceptance_surfaces(acceptance_report: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        surface_key: acceptance_report[surface_key]
        for surface_key in REQUIRED_SURFACE_CONTRACTS
    }


__all__ = ["load_acceptance_surfaces", "report_has_required_inputs"]
