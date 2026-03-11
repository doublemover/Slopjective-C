from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-D001" / "block_runtime_api_object_layout_contract_summary.test.json"
CONTRACT_ID = "objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1"


def test_m261_d001_checker_emits_summary() -> None:
    subprocess.run(
        [
            sys.executable,
            str(CHECKER),
            "--skip-dynamic-probes",
            "--summary-out",
            str(SUMMARY),
        ],
        cwd=ROOT,
        check=True,
    )
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M261-D002"
    assert payload["dynamic_probes_executed"] is False
