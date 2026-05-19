"""Summary payload construction and publication."""

from __future__ import annotations

from datetime import datetime, timezone
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .constants import ACCEPTANCE_REPORT, INTEGRATION_REPORT, REPORT_PATH, REQUIRED_CASES
from .constants import REQUIRED_SURFACE_CONTRACTS, SUMMARY_CONTRACT_ID


def _child_report_paths(
    acceptance_report: dict[str, Any] | None,
    integration_report: dict[str, Any] | None,
) -> list[str]:
    child_report_paths = []
    if isinstance(acceptance_report, dict):
        child_report_paths.append(repo_rel(ACCEPTANCE_REPORT))
    if isinstance(integration_report, dict):
        child_report_paths.append(repo_rel(INTEGRATION_REPORT))
    return child_report_paths


def build_summary_payload(
    *,
    acceptance_report: dict[str, Any] | None,
    integration_report: dict[str, Any] | None,
    surface_source: str,
    integration_surface_comparison: str,
    live_run_dir: str | None,
    package_loading_surface: dict[str, Any],
    abi_surface: dict[str, Any],
    implementation_surface: dict[str, Any],
) -> dict[str, Any]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_interop_conformance.py",
        "surface_source": surface_source,
        "integration_surface_comparison": integration_surface_comparison,
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": _child_report_paths(acceptance_report, integration_report),
        "live_case_run_dir": live_run_dir,
        "runtime_package_loading_module_identity_semantics_surface": package_loading_surface,
        "runtime_package_loader_bridge_abi_surface": abi_surface,
        "runtime_package_loading_interop_implementation_surface": implementation_surface,
    }


def write_summary(payload: dict[str, Any]) -> None:
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")


__all__ = ["build_summary_payload", "write_summary"]
