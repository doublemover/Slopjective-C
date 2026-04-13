from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "build_objc3c_manifest_object_ir_truth_gate.py"
SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "manifest-object-ir-truth-gate"
    / "manifest_object_ir_truth_gate_summary.json"
)


def test_manifest_object_ir_truth_gate_report_is_current() -> None:
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
    assert summary["counts"]["required_artifact_count"] == 12
    assert summary["counts"]["deterministic_artifact_count"] == 10
    assert summary["counts"]["required_object_section_count"] == 12
    assert summary["checks"]["deterministic_artifact_hashes"] is True
    assert summary["checks"]["negative_diagnostics_deterministic"] is True
    assert summary["checks"]["no_source_truth_under_tmp"] is True
    assert summary["positive_runs"]["run1"]["ir_tokens"][
        "manifest_object_ir_truth_gate = contract=objc3c.manifest.object.ir.truth.gate.v1"
    ] is True
