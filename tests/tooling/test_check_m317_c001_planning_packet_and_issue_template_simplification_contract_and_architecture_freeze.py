from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_c001_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze_contract.json"


def test_contract_freezes_required_sections() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert payload["issue_body_contract"]["required_sections"][-1] == "<!-- EXECUTION-ORDER-START -->"
    assert "## Validation posture" in payload["issue_body_contract"]["required_sections"]
    assert "## Intent" in payload["planning_packet_contract"]["required_sections"]
    assert "## Next issue" in payload["planning_packet_contract"]["required_sections"]


def test_validation_postures_and_prohibited_defaults_are_present() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    postures = {item["code"] for item in payload["validation_postures"]}
    assert postures == {"shared_acceptance_harness", "static_policy_guard", "migration_bridge", "generator_contract_only"}
    prohibited = payload["prohibited_defaults"]
    assert any("dedicated checker" in item for item in prohibited)
    assert any("Required deliverables boilerplate" in item for item in prohibited)
