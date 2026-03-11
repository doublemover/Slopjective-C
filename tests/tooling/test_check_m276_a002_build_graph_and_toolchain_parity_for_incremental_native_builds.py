from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-A002" / "build_graph_and_toolchain_parity_summary.test.json"
CONTRACT_ID = "objc3c-native-build-graph-toolchain-parity/m276-a002-v1"


def test_m276_a002_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(TEST_SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(TEST_SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_issue"] == "M276-C001"
    assert payload["inventory_summary"]["missing_in_cmake"] == []
    assert payload["inventory_summary"]["extra_in_cmake"] == []
