from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m260" / "M260-D001" / "runtime_memory_management_api_contract_summary.json"
CONTRACT_ID = "objc3c-runtime-memory-management-api-freeze/m260-d001-v1"
MODE = "m260-d001-runtime-memory-management-api-freeze-v1"


def test_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["mode"] == MODE
    assert payload["dynamic_probes_executed"] is False
    assert payload["dynamic_case"]["skipped"] is True


def test_checker_fails_closed_when_packet_drifts(tmp_path: Path) -> None:
    packet = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_d001_runtime_memory_management_api_contract_and_architecture_freeze_packet.md"
    drift_packet = tmp_path / "packet.md"
    drift_packet.write_text(
        packet.read_text(encoding="utf-8").replace("Packet: `M260-D001`", "Packet: `M260-D001-DRIFT`", 1),
        encoding="utf-8",
    )
    summary = tmp_path / "summary.json"
    completed = subprocess.run(
        [
            sys.executable,
            str(SCRIPT),
            "--skip-dynamic-probes",
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 1
    payload = json.loads(summary.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M260-D001-PKT-03" for failure in payload["failures"])
