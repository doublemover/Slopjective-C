"""Release-candidate conformance orchestration."""

from __future__ import annotations

from typing import Any

from .artifact_checks import (
    build_child_report_paths,
    compare_integration_surfaces,
    validate_live_results,
    validate_release_candidate_surface_relationships,
    validate_surface_contracts,
)
from .command_execution import collect_live_results
from .config import ACCEPTANCE_REPORT, INTEGRATION_REPORT
from .io import load_json
from .reporting import build_summary_payload, write_summary_report
from .surfaces import build_live_surfaces


def select_release_candidate_surfaces(
    _acceptance_report: Any,
) -> tuple[dict[str, dict[str, Any]], str, str | None]:
    results, live_run_dir = collect_live_results()
    validate_live_results(results)
    return build_live_surfaces(results), "live-targeted-cases", live_run_dir


def run_release_candidate_conformance() -> dict[str, Any]:
    acceptance_report = load_json(ACCEPTANCE_REPORT)
    integration_report = load_json(INTEGRATION_REPORT)
    surfaces, surface_source, live_run_dir = select_release_candidate_surfaces(
        acceptance_report
    )

    validate_surface_contracts(surfaces)
    integration_surface_comparison = compare_integration_surfaces(
        integration_report,
        surfaces,
    )
    validate_release_candidate_surface_relationships(surfaces)

    payload = build_summary_payload(
        surface_source=surface_source,
        integration_surface_comparison=integration_surface_comparison,
        child_report_paths=build_child_report_paths(acceptance_report, integration_report),
        live_run_dir=live_run_dir,
        surfaces=surfaces,
    )
    write_summary_report(payload)
    return payload


__all__ = [
    "run_release_candidate_conformance",
    "select_release_candidate_surfaces",
]
