from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-B001" / "object_model_semantic_rules_contract_summary.json"


def test_m256_b001_checker_passes_on_repository_sources() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m256-b001-object-model-semantic-rules-contract-v1"
    assert payload["contract_id"] == "objc3c-object-model-semantic-rules/m256-b001-v1"
    assert payload["ok"] is True
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_m256_b001_checker_fails_closed_when_packet_contract_id_drifts(tmp_path: Path) -> None:
    source_packet = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_b001_object_model_semantic_rules_contract_and_architecture_freeze_packet.md"
    drift_packet = tmp_path / "packet.md"
    drift_packet.write_text(
        source_packet.read_text(encoding="utf-8").replace(
            "`objc3c-object-model-semantic-rules/m256-b001-v1`",
            "`objc3c-object-model-semantic-rules/m256-b001-drift`",
            1,
        ),
        encoding="utf-8",
    )
    summary_out = tmp_path / "summary.json"
    completed = subprocess.run(
        [
            sys.executable,
            str(CHECKER),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_out),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M256-B001-DOC-PKT-04" for failure in payload["failures"])
