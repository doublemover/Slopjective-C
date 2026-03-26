from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c001_task_runner_and_workflow_api_contract_and_architecture_freeze_contract.json"


def test_contract_freezes_compile_as_only_pass_through_action() -> None:
    payload = json.loads(CONTRACT_JSON.read_text(encoding="utf-8"))
    assert payload["pass_through_actions"] == ["compile-objc3c"]


def test_contract_matches_public_script_count() -> None:
    payload = json.loads(CONTRACT_JSON.read_text(encoding="utf-8"))
    assert len(payload["public_workflow_api"]) == 17
