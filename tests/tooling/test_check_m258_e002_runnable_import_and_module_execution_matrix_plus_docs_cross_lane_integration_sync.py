from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
CONTRACT_ID = "objc3c-runnable-import-module-execution-matrix/m258-e002-v1"


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
    dynamic = payload["dynamic_probes"]
    assert dynamic["skipped"] is False
    assert dynamic["consumer"]["cross_module_link_plan"]["module_names_lexicographic"] == [
        "runtimePackagingConsumer",
        "runtimePackagingProvider",
    ]
    probe = dynamic["probe"]["probe_payload"]
    assert probe["startup_registered_image_count"] == 2
    assert probe["imported_provider_class_value"] != 0
    assert probe["imported_provider_protocol_value"] != 0
    assert probe["local_consumer_class_value"] != 0
    assert probe["post_replay_registered_image_count"] == 2
    assert probe["post_replay_imported_provider_class_value"] == probe["imported_provider_class_value"]
    assert probe["post_replay_imported_provider_protocol_value"] == probe["imported_provider_protocol_value"]
    assert probe["post_replay_local_consumer_class_value"] == probe["local_consumer_class_value"]
