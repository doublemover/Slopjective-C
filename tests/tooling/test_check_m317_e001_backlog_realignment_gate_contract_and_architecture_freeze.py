from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_e001_backlog_realignment_gate_contract_and_architecture_freeze_contract.json"


def test_gate_contract_lists_predecessor_summaries() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert set(payload["required_predecessor_summaries"].keys()) == {"M317-A001", "M317-A002", "M317-B001", "M317-B002", "M317-B003", "M317-C001", "M317-C002", "M317-D001"}


def test_gate_contract_freezes_pre_closeout_state() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert payload["pre_closeout_milestone_state"]["milestone_number"] == 398
    assert payload["pre_closeout_milestone_state"]["expected_open_issue_numbers"] == [7833, 7834]
