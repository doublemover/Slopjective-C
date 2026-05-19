"""Runnable interop conformance checker internals."""

from __future__ import annotations

from .assertions import expect
from .cli import load_json, main, write_json_file
from .constants import (
    ACCEPTANCE_REPORT,
    INTEGRATION_REPORT,
    REPORT_PATH,
    REQUIRED_CASES,
    REQUIRED_SURFACE_CONTRACTS,
    ROOT,
    SUMMARY_CONTRACT_ID,
)
from .live_cases import build_live_surfaces, collect_live_results, runtime_acceptance
from .report_selection import load_acceptance_surfaces, report_has_required_inputs
from .summary import build_summary_payload, write_summary
from .validation import (
    integration_surface_comparison,
    validate_live_results,
    validate_required_surface_contracts,
    validate_runtime_package_loading_surfaces,
)


__all__ = [
    "ACCEPTANCE_REPORT",
    "INTEGRATION_REPORT",
    "REPORT_PATH",
    "REQUIRED_CASES",
    "REQUIRED_SURFACE_CONTRACTS",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "build_live_surfaces",
    "build_summary_payload",
    "collect_live_results",
    "expect",
    "integration_surface_comparison",
    "load_acceptance_surfaces",
    "load_json",
    "main",
    "report_has_required_inputs",
    "runtime_acceptance",
    "validate_live_results",
    "validate_required_surface_contracts",
    "validate_runtime_package_loading_surfaces",
    "write_json_file",
    "write_summary",
]
