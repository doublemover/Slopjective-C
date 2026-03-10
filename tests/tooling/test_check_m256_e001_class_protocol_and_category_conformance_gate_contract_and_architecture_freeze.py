from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    ROOT
    / "scripts"
    / "check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m256"
    / "M256-E001"
    / "class_protocol_category_conformance_gate_summary.json"
)


def test_m256_e001_checker() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr

    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert summary["ok"] is True
    assert summary["contract_id"] == "objc3c-executable-class-protocol-category-conformance-gate/m256-e001-v1"
    assert summary["evidence_model"] == "a003-b004-c003-d004-summary-chain"
    assert (
        summary["execution_boundary_model"]
        == "runnable-class-protocol-category-evidence-consumes-source-sema-lowering-and-runtime-proofs"
    )
    assert summary["upstream_evidence"]["m256_a003"]["contract_id"] == "objc3c-executable-protocol-category-source-closure/m256-a003-v1"
    assert summary["upstream_evidence"]["m256_b004"]["contract_id"] == "objc3c-inheritance-override-realization-legality/m256-b004-v1"
    assert summary["upstream_evidence"]["m256_c003"]["contract_id"] == "objc3c-executable-realization-records/m256-c003-v1"
    assert summary["upstream_evidence"]["m256_d004"]["contract_id"] == "objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1"
