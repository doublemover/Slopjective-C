from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m258_c002_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
CONTRACT_ID = "objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1"


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
    assert payload["contract_id"] == CONTRACT_ID
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
    downstream = payload["dynamic_probes"]["downstream"]
    surface = downstream["manifest_surface"]
    assert surface["contract_id"] == CONTRACT_ID
    assert surface["serialized_metadata_rehydration_landed"] is True
    assert surface["artifact_reuse_landed"] is True
    assert surface["downstream_module_consumption_ready"] is True
    assert len(surface["reused_module_names_lexicographic"]) == 3
