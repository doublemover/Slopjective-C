from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPTS_ROOT = Path(__file__).resolve().parents[2] / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

SCRIPT_PATH = SCRIPTS_ROOT / "build_full_envelope_claimability_contracts.py"
SPEC = importlib.util.spec_from_file_location(
    "build_full_envelope_claimability_contracts", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/build_full_envelope_claimability_contracts.py"
    )
contracts = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contracts
SPEC.loader.exec_module(contracts)


def test_public_claim_class_derivation_is_source_owned() -> None:
    assert (
        contracts.public_claim_class_for_rollout("stable", True)
        == contracts.PUBLIC_CLAIM_PRODUCTION_STRENGTH
    )
    assert (
        contracts.public_claim_class_for_rollout("candidate", False)
        == contracts.PUBLIC_CLAIM_CANDIDATE_SCOPED
    )
    assert (
        contracts.public_claim_class_for_rollout("preview", False)
        == contracts.PUBLIC_CLAIM_PREVIEW_ONLY
    )
    assert (
        contracts.public_claim_class_for_rollout("stable", False)
        == contracts.PUBLIC_CLAIM_PREVIEW_ONLY
    )


def test_dashboard_owner_fields_cover_release_artifacts_and_public_summary() -> None:
    assert "dashboard_release_blocker_projection" in contracts.DASHBOARD_DECISION_FIELDS
    assert "release_artifacts" in contracts.DASHBOARD_DECISION_FIELDS
    assert (
        "dashboard_blocks_production_strength_claim"
        in contracts.PUBLIC_SUMMARY_DECISION_FIELDS
    )
    assert contracts.RELEASE_ARTIFACT_FIELDS == (
        "release_manifest_path",
        "published_sbom",
        "published_attestation",
        "update_manifest_path",
        "compatibility_report",
        "channel_catalog",
        "trust_report_json",
    )


def test_checked_in_dashboard_schema_requires_owner_decision_fields() -> None:
    root = Path(__file__).resolve().parents[2]
    schema = json.loads(
        (
            root
            / "schemas"
            / "objc3c-full-envelope-dashboard-summary-v1.schema.json"
        ).read_text(encoding="utf-8")
    )
    assert set(contracts.DASHBOARD_DECISION_FIELDS).issubset(schema["required"])
    projection_required = schema["properties"]["dashboard_release_blocker_projection"][
        "required"
    ]
    assert "source_owned_decision_fields" in projection_required
    assert "required_public_summary_fields" in projection_required
