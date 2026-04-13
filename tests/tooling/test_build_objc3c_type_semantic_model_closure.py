from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "build_objc3c_type_semantic_model_closure.py"
SUMMARY = ROOT / "reports" / "claimability" / "type-semantic-model-closure" / "type_semantic_model_closure_summary.json"


def test_type_semantic_model_closure_report_is_current() -> None:
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
    assert SUMMARY.is_file()
