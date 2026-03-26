from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_a002_exception_process_and_ownership_model_source_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-A002" / "exception_process_summary.json"


def test_m318_a002_checker_passes_and_writes_summary() -> None:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["process_contract_id"] == "objc3c-governance-anti-noise-exception-process/m318-a002-v1"
    assert payload["registry_contract_id"] == "objc3c-governance-anti-noise-exception-registry/m318-a002-v1"
    assert payload["active_exception_count"] == 0
    assert payload["allowed_statuses"] == ["active", "expired", "retired", "rejected"]
    assert payload["package_governance"]["governanceOwnerIssue"] == "M318-A002"
