"""Runnable release-candidate conformance implementation package."""

from __future__ import annotations

from .artifact_checks import (
    build_child_report_paths,
    build_report_case_map,
    compare_integration_surfaces,
    expect,
    report_has_required_cases,
    report_has_required_surfaces,
    validate_live_results,
    validate_release_candidate_surface_relationships,
    validate_surface_contracts,
)
from .cli import main
from .command_execution import collect_live_results, runtime_acceptance
from .config import (
    ACCEPTANCE_REPORT,
    DASHBOARD_SCHEMA_PATH,
    INTEGRATION_REPORT,
    LIVE_CASE_ROOT,
    REPORT_PATH,
    REQUIRED_CASES,
    REQUIRED_SURFACE_CONTRACTS,
    RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES,
    ROOT,
    RUNNER_PATH,
    SCRIPTS_ROOT,
    SUMMARY_CONTRACT_ID,
    TARGETED_PROFILE_IDS,
)
from .io import load_json, repo_rel, write_json_file
from .orchestration import run_release_candidate_conformance
from .reporting import build_summary_payload, write_summary_report
from .surfaces import build_live_surfaces

__all__ = [
    "ACCEPTANCE_REPORT",
    "DASHBOARD_SCHEMA_PATH",
    "INTEGRATION_REPORT",
    "LIVE_CASE_ROOT",
    "REPORT_PATH",
    "REQUIRED_CASES",
    "REQUIRED_SURFACE_CONTRACTS",
    "RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES",
    "ROOT",
    "RUNNER_PATH",
    "SCRIPTS_ROOT",
    "SUMMARY_CONTRACT_ID",
    "TARGETED_PROFILE_IDS",
    "build_child_report_paths",
    "build_live_surfaces",
    "build_report_case_map",
    "build_summary_payload",
    "collect_live_results",
    "compare_integration_surfaces",
    "expect",
    "load_json",
    "main",
    "repo_rel",
    "report_has_required_cases",
    "report_has_required_surfaces",
    "run_release_candidate_conformance",
    "runtime_acceptance",
    "validate_live_results",
    "validate_release_candidate_surface_relationships",
    "validate_surface_contracts",
    "write_json_file",
    "write_summary_report",
]
