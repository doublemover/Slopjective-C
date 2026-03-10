from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m258_b001_cross_module_semantic_preservation_contract_and_architecture_freeze.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
CONTRACT_ID = "objc3c-cross-module-runtime-metadata-semantic-preservation/m258-b001-v1"


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
    assert payload["dynamic_probes"]["class_fixture"]["module_name"] == "runtimeMetadataClassRecords"
    assert payload["dynamic_probes"]["category_fixture"]["module_name"] == "runtimeMetadataCategoryRecords"
    assert payload["dynamic_probes"]["class_fixture"]["class_record_count"] > 0
    assert payload["dynamic_probes"]["category_fixture"]["category_record_count"] > 0
