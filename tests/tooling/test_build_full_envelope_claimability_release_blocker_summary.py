from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

SCRIPTS_ROOT = Path(__file__).resolve().parents[2] / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "build_full_envelope_claimability_release_blocker_summary.py"
)
SPEC = importlib.util.spec_from_file_location(
    "build_full_envelope_claimability_release_blocker_summary", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/build_full_envelope_claimability_release_blocker_summary.py"
    )
builder = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = builder
SPEC.loader.exec_module(builder)

FIXTURE_ROOT = (
    Path(__file__).resolve().parent
    / "fixtures"
    / "full_envelope_claimability_dashboard_blockers"
)


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def configure_paths(monkeypatch: pytest.MonkeyPatch, tmp_path: Path, policy: str) -> None:
    out_dir = tmp_path / "release-blockers"
    monkeypatch.setattr(builder, "ROOT", FIXTURE_ROOT)
    monkeypatch.setattr(builder, "POLICY_CONTRACT_PATH", FIXTURE_ROOT / policy)
    monkeypatch.setattr(builder, "RUNBOOK_PATH", FIXTURE_ROOT / "runbook.md")
    monkeypatch.setattr(builder, "OUT_DIR", out_dir)
    monkeypatch.setattr(builder, "JSON_OUT", out_dir / "release_blocker_summary.json")
    monkeypatch.setattr(builder, "MD_OUT", out_dir / "release_blocker_summary.md")


def test_release_blockers_include_dashboard_projection_blocker(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    configure_paths(monkeypatch, tmp_path, "policy.json")

    code = builder.main()

    assert code == 0
    payload = read_json(tmp_path / "release-blockers" / "release_blocker_summary.json")
    assert payload["status"] == "PASS"
    assert payload["policy_triggered_blocker_count"] == 1
    assert payload["dashboard_triggered_blocker_count"] == 1
    assert payload["current_rollout_class"] == "preview"
    assert payload["production_strength_claimable"] is False
    assert "performance-claim-not-ready" in payload["triggered_blockers"]
    assert "claimability-dashboard-not-production-strength" in payload[
        "triggered_blockers"
    ]
    projection = payload["dashboard_release_blocker_projection"]
    assert projection["public_claim_class"] == "preview-only"
    assert projection["blocks_production_strength_claim"] is True
    assert "dashboard_release_blocker_projection" in projection[
        "required_dashboard_fields"
    ]
    assert "dashboard_blocks_production_strength_claim" in projection[
        "required_public_summary_fields"
    ]
    assert "release_artifacts" in projection["source_owned_decision_fields"]
    assert payload["checks"]["triggered_blockers_include_dashboard_projection"] is True
    assert payload["checks"]["dashboard_projection_requires_source_owned_decisions"] is True


def test_release_blocker_policy_requires_dashboard_projection(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    configure_paths(monkeypatch, tmp_path, "policy_missing_dashboard_projection.json")

    with pytest.raises(RuntimeError, match="dashboard_release_blocker_projection"):
        builder.main()
