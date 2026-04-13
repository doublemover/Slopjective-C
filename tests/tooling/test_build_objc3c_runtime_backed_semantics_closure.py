from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "build_objc3c_runtime_backed_semantics_closure.py"
SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "runtime-backed-semantics-closure"
    / "runtime_backed_semantics_closure_summary.json"
)


def test_runtime_backed_semantics_closure_report_is_current() -> None:
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
    assert summary["counts"]["positive_fixture_count"] == 7
    assert summary["counts"]["negative_fixture_count"] == 4
    assert summary["counts"]["runtime_helper_symbol_count"] == 36
    assert summary["checks"]["no_source_truth_under_tmp"] is True
    assert summary["required_ir_tokens"][
        "runtime_backed_semantics_closure = contract=objc3c.runtime.backed.semantics.closure.v1"
    ] is True
    assert summary["negative_compile"]["task_group_without_scope"][
        "expected_codes_present"
    ] is True
