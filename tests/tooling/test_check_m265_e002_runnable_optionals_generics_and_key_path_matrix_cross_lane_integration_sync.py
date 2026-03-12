from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m265_e002_runnable_optionals_generics_and_key_path_matrix_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-E002" / "runnable_type_surface_closeout_summary.json"


def test_m265_e002_checker() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == "objc3c-runnable-type-surface-closeout/m265-e002-v1"
    assert payload["next_issue"] == "M266-A001"

    runtime_rows = {row["row"]: row for row in payload["runtime_rows"]}
    assert runtime_rows["optional-send-short-circuit"]["run_exit"] == 0
    assert runtime_rows["optional-binding-refinement"]["run_exit"] == 36
    assert runtime_rows["optional-member-access"]["run_exit"] == 9
    assert runtime_rows["typed-keypath-runtime"]["run_exit"] == 11

    generic_row = payload["generic_preservation_row"]
    assert generic_row["status"] == "ok"
    assert generic_row["generic_metadata_abi_sites"] == 1
    assert generic_row["generic_metadata_replay_key_present"] == 1
