from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-A002" / "class_metaclass_declaration_completeness_and_inheritance_modeling_summary.json"


def test_m256_a002_checker_passes_on_repository_sources() -> None:
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
    assert payload["mode"] == "m256-a002-class-metaclass-declaration-completeness-v1"
    assert payload["contract_id"] == "objc3c-executable-class-metaclass-source-closure/m256-a002-v1"
    assert payload["ok"] is True
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []
    assert payload["dynamic_probe"]["manifest_path"].endswith("module.manifest.json")
    assert payload["dynamic_probe"]["ir_path"].endswith("module.ll")


def test_m256_a002_checker_fails_closed_when_packet_contract_id_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "packet.md"
    drift_packet.write_text(
        (
            ROOT
            / "spec"
            / "planning"
            / "compiler"
            / "m256"
            / "m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation_packet.md"
        )
        .read_text(encoding="utf-8")
        .replace(
            "`objc3c-executable-class-metaclass-source-closure/m256-a002-v1`",
            "`objc3c-executable-class-metaclass-source-closure/m256-a002-drift`",
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
            "--skip-dynamic-probes",
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
    assert any(failure["check_id"] == "M256-A002-DOC-PKT-03" for failure in payload["failures"])
