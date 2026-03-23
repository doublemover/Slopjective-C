from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m267" / "M267-C003" / "result_and_bridging_artifact_replay_completion_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"


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

    assert completed.returncode == 0, completed.stderr
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == "objc3c-part6-result-and-bridging-artifact-replay/m267-c003-v1"
    assert payload["dynamic_probes"]["skipped"] is True


def test_checker_passes_dynamic(tmp_path: Path) -> None:
    if not NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")

    summary_out = tmp_path / "summary.json"
    completed = run_checker("--summary-out", str(summary_out))

    assert completed.returncode == 0, completed.stderr
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes"]["skipped"] is False
    producer = payload["dynamic_probes"]["provider"]
    consumer = payload["dynamic_probes"]["consumer"]
    assert producer["module_name"] == "m267_c003_part6_artifact_replay_producer"
    replay_surface = consumer["replay_surface"]
    assert replay_surface["imported_module_names_lexicographic"] == ["m267_c003_part6_artifact_replay_producer"]
    assert replay_surface["separate_compilation_replay_ready"] is True
