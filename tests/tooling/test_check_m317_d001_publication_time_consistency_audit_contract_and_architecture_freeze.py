from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_d001_publication_time_consistency_audit_contract_and_architecture_freeze_contract.json"


def test_contract_freezes_audit_facets() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    facets = set(payload["audit_facets"])
    assert "open_issue_label_completeness" in facets
    assert "post_cleanup_dependency_rewrite_coverage" in facets
    assert "idempotent_apply_behavior" in facets


def test_contract_freezes_report_keys_and_surfaces() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert "checked_issues" in payload["required_report_keys"]
    assert "updated_entities" in payload["required_report_keys"]
    assert len(payload["future_implementation_surfaces"]) == 5
