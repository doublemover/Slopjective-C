"""Summary rendering for the external validation source-surface checker."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .paths import EXPECTED_ARTIFACT_ROOT, EXPECTED_REPORT_ROOT
from .source_model import EXPECTED_FAMILY_IDS, SUMMARY_CONTRACT_ID, SourceSurfaceValidation


def render_summary(
    validation: SourceSurfaceValidation,
    *,
    source_surface_path: Path,
) -> dict[str, Any]:
    required_paths = validation.required_paths
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_surface": repo_rel(source_surface_path),
        "runbook": required_paths["runbook"],
        "source_root": required_paths["source_root"],
        "source_readme": required_paths["source_readme"],
        "source_check_script": required_paths["source_check_script"],
        "trust_policy": required_paths["trust_policy"],
        "intake_manifest": required_paths["intake_manifest"],
        "quarantine_manifest": required_paths["quarantine_manifest"],
        "artifact_surface": required_paths["artifact_surface"],
        "workflow_surface": required_paths["workflow_surface"],
        "checked_in_roots": list(validation.checked_roots),
        "expected_family_ids": list(EXPECTED_FAMILY_IDS),
        "artifact_root": EXPECTED_ARTIFACT_ROOT,
        "report_root": EXPECTED_REPORT_ROOT,
        "family_summaries": validation.family_summaries,
        "intake_entry_summaries": validation.intake_entry_summaries,
        "quarantine_entry_summaries": validation.quarantine_entry_summaries,
        "checked_path_count": len(validation.checked_paths),
        "checked_paths": validation.checked_paths,
    }
