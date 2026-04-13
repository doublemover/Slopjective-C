from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "build_objc3c_object_model_ir_lowering_closure.py"
SUMMARY = ROOT / "reports" / "claimability" / "object-model-ir-lowering" / "object_model_ir_lowering_summary.json"


def test_object_model_ir_lowering_closure_report_is_current() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "status: PASS" in result.stdout
    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert summary["ir_offsets_by_property"]["token"] == 8
    assert summary["ir_offsets_by_property"]["childFlag"] == 16
    assert summary["expected_layout"]["childFlag"]["owner_size"] == 24
