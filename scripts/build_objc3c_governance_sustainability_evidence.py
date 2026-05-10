#!/usr/bin/env python3
"""Build the machine-owned governance sustainability evidence artifact."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command

try:
    from governance_sustainability_evidence_contracts import (
        GovernanceSustainabilityEvidenceInputs,
        build_governance_sustainability_evidence_payloads,
    )
except ModuleNotFoundError:
    from scripts.governance_sustainability_evidence_contracts import (
        GovernanceSustainabilityEvidenceInputs,
        build_governance_sustainability_evidence_payloads,
    )


ROOT = Path(__file__).resolve().parents[1]


ARTIFACT_CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "artifact_contract.json"
WAIVER_REGISTRY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "waiver_registry.json"
INTEGRATION_CHECK = ROOT / "scripts" / "check_objc3c_governance_sustainability_integration.py"
EVIDENCE_ARTIFACT = ROOT / "tmp" / "artifacts" / "governance-sustainability" / "governance-sustainability-evidence.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "governance-sustainability" / "evidence-summary.json"


def ensure_integration() -> None:
    result = subprocess.run(
        python_script_command(INTEGRATION_CHECK),
        cwd=ROOT,
        text=True,
        capture_output=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        raise RuntimeError("governance sustainability integration failed during evidence generation")


def load_optional_summary(relative_path: str) -> dict[str, Any]:
    path = ROOT / relative_path
    return load_json(path) if path.is_file() else {}


def main() -> int:
    ensure_integration()
    contract = load_json(ARTIFACT_CONTRACT_PATH)
    waiver_registry = load_json(WAIVER_REGISTRY_PATH)
    generated_reports = [str(path) for path in contract.get("generated_reports", [])]
    report_payloads = {path: load_optional_summary(path) for path in generated_reports}

    payloads = build_governance_sustainability_evidence_payloads(
        GovernanceSustainabilityEvidenceInputs(
            contract=contract,
            waiver_registry=waiver_registry,
            generated_reports=generated_reports,
            report_payloads=report_payloads,
            artifact_contract_path=repo_rel(ARTIFACT_CONTRACT_PATH),
            evidence_artifact_path=repo_rel(EVIDENCE_ARTIFACT),
        )
    )

    EVIDENCE_ARTIFACT.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(EVIDENCE_ARTIFACT, payloads.evidence)
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payloads.summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"evidence_artifact: {repo_rel(EVIDENCE_ARTIFACT)}")
    print(
        "objc3c-governance-sustainability-evidence: PASS"
        if not payloads.failures
        else "objc3c-governance-sustainability-evidence: FAIL"
    )
    return 0 if not payloads.failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
