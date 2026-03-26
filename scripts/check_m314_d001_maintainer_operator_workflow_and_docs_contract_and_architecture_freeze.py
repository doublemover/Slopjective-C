#!/usr/bin/env python3
"""Checker for M314-D001 maintainer operator workflow and docs contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-D001" / "maintainer_operator_workflow_docs_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_maintainer_operator_workflow_and_docs_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_d001_maintainer_operator_workflow_and_docs_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_d001_maintainer_operator_workflow_and_docs_contract_and_architecture_freeze_contract.json"
PACKAGE_JSON = ROOT / "package.json"
README = ROOT / "README.md"
RUNBOOK = ROOT / "docs" / "runbooks" / "objc3c_maintainer_workflows.md"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = json.loads(read_text(CONTRACT_JSON))
    package = json.loads(read_text(PACKAGE_JSON))
    package_text = read_text(PACKAGE_JSON)
    operator_docs = package.get("objc3cOperatorWorkflows", {})
    readme = read_text(README)
    runbook = read_text(RUNBOOK)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-maintainer-operator-workflow-docs/m314-d001-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-D001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("machine-readable operator-doc metadata" in expectations, str(EXPECTATIONS_DOC), "M314-D001-EXP-02", "expectations missing machine-readable metadata note", failures)
    checks_passed += require("canonical operator-doc map" in packet, str(PACKET_DOC), "M314-D001-PKT-01", "packet missing operator-doc map summary", failures)
    checks_passed += require("Next issue: `M314-E001`." in packet, str(PACKET_DOC), "M314-D001-PKT-02", "packet missing next issue", failures)

    checks_total += 9
    checks_passed += require(contract.get("mode") == "m314-d001-maintainer-operator-workflow-docs-v1", str(CONTRACT_JSON), "M314-D001-CON-01", "mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == "objc3c-cleanup-maintainer-operator-workflow-docs/m314-d001-v1", str(CONTRACT_JSON), "M314-D001-CON-02", "contract id drifted", failures)
    checks_passed += require(contract.get("depends_on") == "M314-C003", str(CONTRACT_JSON), "M314-D001-CON-03", "dependency drifted", failures)
    checks_passed += require(contract.get("supported_compiler_root") == "native/objc3c", str(CONTRACT_JSON), "M314-D001-CON-04", "supported compiler root drifted", failures)
    checks_passed += require(contract.get("canonical_docs", {}).get("maintainer_runbook") == "docs/runbooks/objc3c_maintainer_workflows.md", str(CONTRACT_JSON), "M314-D001-CON-05", "maintainer runbook path drifted", failures)
    checks_passed += require(contract.get("canonical_docs", {}).get("public_command_runbook") == "docs/runbooks/objc3c_public_command_surface.md", str(CONTRACT_JSON), "M314-D001-CON-06", "public command runbook path drifted", failures)
    checks_passed += require("internal maintainer surface" in contract.get("internal_surface_rule", ""), str(CONTRACT_JSON), "M314-D001-CON-07", "internal surface rule drifted", failures)
    checks_passed += require("archival only" in contract.get("retired_surface_rule", ""), str(CONTRACT_JSON), "M314-D001-CON-08", "retired surface rule drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M314-E001", str(CONTRACT_JSON), "M314-D001-CON-09", "next issue drifted", failures)

    checks_total += 8
    checks_passed += require(operator_docs.get("contractId") == contract["contract_id"], str(PACKAGE_JSON), "M314-D001-PKG-01", "package operator workflow contract drifted", failures)
    checks_passed += require(operator_docs.get("maintainerRunbook") == contract["canonical_docs"]["maintainer_runbook"], str(PACKAGE_JSON), "M314-D001-PKG-02", "package maintainer runbook drifted", failures)
    checks_passed += require(operator_docs.get("publicCommandRunbook") == contract["canonical_docs"]["public_command_runbook"], str(PACKAGE_JSON), "M314-D001-PKG-03", "package public command runbook drifted", failures)
    checks_passed += require(operator_docs.get("supportedCompilerRoot") == "native/objc3c", str(PACKAGE_JSON), "M314-D001-PKG-04", "package supported compiler root drifted", failures)
    checks_passed += require("Maintainers should use `docs/runbooks/objc3c_maintainer_workflows.md`" in readme, str(README), "M314-D001-RD-01", "README missing maintainer runbook link", failures)
    checks_passed += require("## Canonical Docs" in runbook and "## Public-First Rule" in runbook and "## Common Maintainer Flows" in runbook, str(RUNBOOK), "M314-D001-DOC-01", "maintainer runbook missing core sections", failures)
    checks_passed += require("docs/runbooks/objc3c_public_command_surface.md" in runbook or "objc3c_public_command_surface.md" in runbook, str(RUNBOOK), "M314-D001-DOC-02", "maintainer runbook missing public command runbook link", failures)
    checks_passed += require('"check:objc3c:m314-d001-maintainer-operator-workflow-and-docs-contract-and-architecture-freeze"' in package_text and '"check:objc3c:m314-d001-lane-d-readiness"' in package_text, str(PACKAGE_JSON), "M314-D001-PKG-05", "package missing issue-local scripts", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": contract["mode"],
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "maintainer_runbook": contract["canonical_docs"]["maintainer_runbook"],
        "public_command_runbook": contract["canonical_docs"]["public_command_runbook"],
        "next_issue": "M314-E001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-D001 maintainer/operator docs checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
