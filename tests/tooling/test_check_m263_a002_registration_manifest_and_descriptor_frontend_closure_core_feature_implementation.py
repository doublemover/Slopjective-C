from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
CONTRACT_ID = "objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1"


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

    explicit = payload["dynamic_probes"]["explicit"]
    default = payload["dynamic_probes"]["default"]
    assert explicit["registration_descriptor_identifier"] == "FrontendDescriptorRegistration"
    assert explicit["image_root_identifier"] == "FrontendDescriptorImageRoot"
    assert explicit["registration_descriptor_identity_source"] == "source-pragma"
    assert explicit["image_root_identity_source"] == "source-pragma"
    assert explicit["artifact"] == "module.runtime-registration-descriptor.json"
    assert default["registration_descriptor_identifier"] == "DescriptorFrontendAuto_registration_descriptor"
    assert default["image_root_identifier"] == "DescriptorFrontendAuto_image_root"
    assert default["registration_descriptor_identity_source"] == "module-derived-default"
    assert default["image_root_identity_source"] == "module-derived-default"
    assert default["artifact"] == "module.runtime-registration-descriptor.json"
