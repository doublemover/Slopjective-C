"""Report payload rendering and persistence."""

from __future__ import annotations

from datetime import datetime, timezone
from typing import Any

from .config import (
    REPORT_PATH,
    REQUIRED_CASES,
    REQUIRED_SURFACE_CONTRACTS,
    RUNNER_PATH,
    SUMMARY_CONTRACT_ID,
)
from .io import repo_rel, write_json_file


def build_summary_payload(
    *,
    surface_source: str,
    integration_surface_comparison: str,
    child_report_paths: list[str],
    live_run_dir: str | None,
    surfaces: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    claim_surface = surfaces["runtime_release_candidate_claim_abi_surface"]
    evidence_surface = surfaces["runtime_current_release_evidence_owner_payload_surface"]
    strict_claim_surface = surfaces[
        "runtime_strict_profile_claim_implementation_surface"
    ]
    final_publication_surface = surfaces[
        "runtime_final_claim_publication_deprecated_path_shutdown_surface"
    ]
    dashboard_surface = surfaces[
        "runtime_claim_publication_dashboard_schema_surface"
    ]

    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": RUNNER_PATH,
        "surface_source": surface_source,
        "integration_surface_comparison": integration_surface_comparison,
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": child_report_paths,
        "live_case_run_dir": live_run_dir,
        "runtime_release_candidate_claim_abi_surface": claim_surface,
        "runtime_current_release_evidence_owner_payload_surface": evidence_surface,
        "runtime_strict_profile_claim_implementation_surface": strict_claim_surface,
        "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
            final_publication_surface
        ),
        "runtime_claim_publication_dashboard_schema_surface": dashboard_surface,
    }


def write_summary_report(payload: dict[str, Any]) -> None:
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")


__all__ = ["build_summary_payload", "write_summary_report"]
