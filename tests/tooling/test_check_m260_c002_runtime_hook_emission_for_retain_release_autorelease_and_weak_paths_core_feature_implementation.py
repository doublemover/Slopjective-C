from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m260" / "M260-C002" / "ownership_runtime_hook_emission_summary.json"
CONTRACT_ID = "objc3c-ownership-runtime-hook-emission/m260-c002-v1"
MODE = "m260-c002-runtime-hook-emission-core-feature-implementation-v1"


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


def test_checker_fails_closed_when_expectations_doc_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    original = (ROOT / "docs" / "contracts" / "m260_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation_c002_expectations.md").read_text(encoding="utf-8")
    drift_doc.write_text(
        original.replace(
            "Contract ID: `objc3c-ownership-runtime-hook-emission/m260-c002-v1`",
            "Contract ID: `objc3c-ownership-runtime-hook-emission/m260-c002-drifted`",
            1,
        ),
        encoding="utf-8",
    )
    summary = tmp_path / "summary.json"
    completed = subprocess.run(
        [
            sys.executable,
            str(SCRIPT),
            "--skip-dynamic-probes",
            "--expectations-doc",
            str(drift_doc),
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
    assert any(failure["check_id"] == "M260-C002-DOC-EXP-02" for failure in payload["failures"])
