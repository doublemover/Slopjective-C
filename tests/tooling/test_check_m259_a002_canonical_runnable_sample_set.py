from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m259_a002_canonical_runnable_sample_set.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
CONTRACT_ID = "objc3c-canonical-runnable-sample-set/m259-a002-v1"


def run_checker(*args: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCRIPT), *args],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def test_checker_passes_static(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    completed = run_checker("--skip-dynamic-probes", "--summary-out", str(summary_out))

    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["dynamic_probes"]["skipped"] is True
    assert payload["next_issue"] == "M259-B001"


def test_checker_passes_dynamic(tmp_path: Path) -> None:
    if not NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")

    summary_out = tmp_path / "summary.json"
    completed = run_checker("--summary-out", str(summary_out))

    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    dynamic = payload["dynamic_probes"]
    probe = dynamic["probe_payload"]

    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert dynamic["skipped"] is False
    assert payload["dependency"]["M259-A001"]["contract_id"] == "objc3c-runnable-sample-surface/m259-a001-v1"
    assert probe["traced_value"] == 13
    assert probe["inherited_value"] == 7
    assert probe["class_value"] == 11
    assert probe["shared_value"] == 19
    assert probe["count_value"] == 37
    assert probe["enabled_value"] == 1
    assert probe["current_value"] == 55
    assert probe["token_value"] == 0
    assert probe["tracer_query"]["matched_attachment_owner_identity"] == "category:Widget(Tracing)"
