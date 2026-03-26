from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
TARGETS = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_b003_future_milestone_dependency_rewrites_for_post_m292_work_edge_case_and_compatibility_completion_targets.json"


def test_targets_cover_all_post_m292_milestones() -> None:
    payload = json.loads(TARGETS.read_text(encoding="utf-8"))
    milestones = payload["milestone_targets"]
    assert len(milestones) == 16
    assert {item["number"] for item in milestones} == set(range(378, 394))
    assert sum(item["expected_open_issue_count"] for item in milestones) == 180


def test_every_target_has_rewrite_snippets() -> None:
    payload = json.loads(TARGETS.read_text(encoding="utf-8"))
    for milestone in payload["milestone_targets"]:
        assert "Corrective dependencies consumed:" in milestone["required_description_snippets"]
        assert milestone["required_issue_body_snippets"][0] == "## Post-cleanup dependency rewrite"
        assert any("`M313`" in snippet for snippet in milestone["required_issue_body_snippets"])
        assert any("`M315`" in snippet for snippet in milestone["required_issue_body_snippets"])
        assert any("`M318`" in snippet for snippet in milestone["required_issue_body_snippets"])
