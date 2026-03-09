from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m263_c002_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-C002" / "pytest_registration_descriptor_lowering_and_multi_image_root_emission_summary.json"


def test_m263_c002_static_contract_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == "objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1"
    assert payload["dynamic_probes_executed"] is False
