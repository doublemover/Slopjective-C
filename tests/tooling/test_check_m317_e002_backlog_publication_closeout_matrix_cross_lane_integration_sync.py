from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_e002_backlog_publication_closeout_matrix_cross_lane_integration_sync_contract.json"


def test_closeout_contract_lists_predecessor_summaries() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert set(payload["required_predecessor_summaries"].keys()) == {"M317-A001", "M317-A002", "M317-B001", "M317-B002", "M317-B003", "M317-C001", "M317-C002", "M317-D001", "M317-E001"}


def test_closeout_contract_freezes_pre_closeout_state() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert payload["pre_closeout_milestone_state"]["milestone_number"] == 398
    assert payload["pre_closeout_milestone_state"]["expected_open_issue_numbers"] == [7834]
    assert payload["next_issue"] == "M313-A001"
