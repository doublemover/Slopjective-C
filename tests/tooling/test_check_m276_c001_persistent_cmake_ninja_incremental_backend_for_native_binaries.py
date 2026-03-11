from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-C001" / "persistent_cmake_ninja_incremental_backend_summary.test.json"
CONTRACT_ID = "objc3c-persistent-cmake-ninja-native-build-backend/m276-c001-v1"


def test_m276_c001_checker_emits_summary() -> None:
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
    assert payload["next_issue"] == "M276-C002"
    assert payload["dynamic_summary"]["compile_commands_path"] == "tmp/build-objc3c-native/compile_commands.json"
    assert payload["dynamic_summary"]["fingerprint_path"] == "tmp/build-objc3c-native/native_build_backend_fingerprint.json"
    assert payload["dynamic_summary"]["incremental_compile_hits"] == 1
    assert payload["dynamic_summary"]["incremental_unrelated_sema_hits"] == 0
    assert payload["dynamic_summary"]["cold_build_dir"].startswith("tmp/reports/m276/M276-C001/cold-build-")
    assert payload["dynamic_summary"]["cold_compile_commands"].endswith("compile_commands.json")
