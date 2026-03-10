from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m258_a002_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
CONTRACT_ID = "objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1"


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
    if not NATIVE_EXE.exists() or not RUNNER_EXE.exists():
        pytest.skip("native frontend and runner are not built")

    summary_out = tmp_path / "summary.json"
    completed = run_checker("--summary-out", str(summary_out))

    assert completed.returncode == 0, completed.stderr
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes"]["skipped"] is False
    assert payload["dynamic_probes"]["parity"] is True
    assert payload["dynamic_probes"]["native_class"]["module_name"] == "runtimeMetadataClassRecords"
    assert payload["dynamic_probes"]["runner_class"]["module_name"] == "runtimeMetadataClassRecords"
    assert payload["dynamic_probes"]["native_category"]["module_name"] == "runtimeMetadataCategoryRecords"
    assert payload["dynamic_probes"]["native_class"]["category_record_count"] == 0
    assert payload["dynamic_probes"]["native_category"]["category_record_count"] > 0
