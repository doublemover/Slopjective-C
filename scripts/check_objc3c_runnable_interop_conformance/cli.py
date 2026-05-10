"""Command-line orchestration for runnable interop conformance."""

from __future__ import annotations

from objc3c_tooling.json_io import load_optional_json_object as load_json
from objc3c_tooling.json_io import write_json_file

from .constants import ACCEPTANCE_REPORT, INTEGRATION_REPORT
from .live_cases import build_live_surfaces, collect_live_results
from .report_selection import load_acceptance_surfaces, report_has_required_inputs
from .summary import build_summary_payload, write_summary
from .validation import (
    integration_surface_comparison,
    validate_live_results,
    validate_required_surface_contracts,
    validate_runtime_package_loading_surfaces,
)


def main() -> int:
    acceptance_report = load_json(ACCEPTANCE_REPORT)
    integration_report = load_json(INTEGRATION_REPORT)

    live_run_dir: str | None = None
    if report_has_required_inputs(acceptance_report):
        surfaces = load_acceptance_surfaces(acceptance_report)
        surface_source = "acceptance-report"
    else:
        results, live_run_dir = collect_live_results()
        validate_live_results(results)
        surfaces = build_live_surfaces(results)
        surface_source = "live-targeted-cases"

    validate_required_surface_contracts(surfaces)
    surface_comparison = integration_surface_comparison(integration_report, surfaces)
    package_loading_surface, abi_surface, implementation_surface = (
        validate_runtime_package_loading_surfaces(surfaces)
    )

    payload = build_summary_payload(
        acceptance_report=acceptance_report,
        integration_report=integration_report,
        surface_source=surface_source,
        integration_surface_comparison=surface_comparison,
        live_run_dir=live_run_dir,
        package_loading_surface=package_loading_surface,
        abi_surface=abi_surface,
        implementation_surface=implementation_surface,
    )
    write_summary(payload)
    return 0


__all__ = ["load_json", "main", "write_json_file"]
