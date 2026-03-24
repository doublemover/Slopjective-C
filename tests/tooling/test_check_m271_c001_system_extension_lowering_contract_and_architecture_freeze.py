from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m271_c001_system_extension_lowering_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m271" / "M271-C001" / "system_extension_lowering_contract_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m271_c001_static_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m271-c001-part8-system-extension-lowering-contract-v1"
    assert payload["contract_id"] == "objc3c-part8-system-extension-lowering-contract/m271-c001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "dynamic_probes_executed" in preserved
