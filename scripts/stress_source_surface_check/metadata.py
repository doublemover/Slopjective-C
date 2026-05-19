"""Top-level stress source-surface metadata checks."""

from __future__ import annotations

from typing import Any

from .constants import SOURCE_CHECK_SCRIPT, SURFACE_CONTRACT_ID, WORKFLOW_SURFACE
from .failures import require


def validate_surface_metadata(surface: dict[str, Any]) -> None:
    require(surface.get("contract_id") == SURFACE_CONTRACT_ID, "stress source surface contract_id drifted")
    require(surface.get("schema_version") == 1, "stress source surface schema_version drifted")
    require(surface.get("runbook") == "docs/runbooks/objc3c_stress_validation.md", "stress source surface runbook drifted")
    require(surface.get("source_root") == "tests/tooling/fixtures/stress", "stress source surface source_root drifted")
    require(surface.get("source_check_script") == SOURCE_CHECK_SCRIPT, "stress source surface source_check_script drifted")
    require(surface.get("safety_policy") == "tests/tooling/fixtures/stress/safety_policy.json", "stress source surface safety_policy drifted")
    require(surface.get("artifact_surface") == "tests/tooling/fixtures/stress/artifact_surface.json", "stress source surface artifact_surface drifted")
    require(surface.get("workflow_surface") == WORKFLOW_SURFACE, "stress source surface workflow_surface drifted")
