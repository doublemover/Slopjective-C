#!/usr/bin/env python3
"""Checker for M313-E002 validation consolidation closeout matrix."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-E002" / "validation_consolidation_closeout_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_validation_consolidation_closeout_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_e002_validation_consolidation_closeout_matrix_cross_lane_integration_sync_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_e002_validation_consolidation_closeout_matrix_cross_lane_integration_sync_contract.json"
PACKAGE_JSON = ROOT / "package.json"


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


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def gh_json(command: Sequence[str], artifact: str, check_id: str, failures: list[Finding]) -> Any | None:
    completed = run_command(command)
    if completed.returncode != 0:
        failures.append(Finding(artifact, check_id, completed.stderr.strip() or completed.stdout.strip() or "gh command failed"))
        return None
    try:
        return json.loads(completed.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, f"{check_id}-JSON", f"invalid JSON: {exc}"))
        return None


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = json.loads(read_text(CONTRACT_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-validation-consolidation-closeout-matrix/m313-e002-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-E002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Pre-closeout milestone state is reduced to the final closeout issue itself: `#7779`." in expectations, str(EXPECTATIONS_DOC), "M313-E002-EXP-02", "expectations missing pre-closeout issue note", failures)
    checks_passed += require("proof chain" in packet, str(PACKET_DOC), "M313-E002-PKT-01", "packet missing proof-chain focus", failures)
    checks_passed += require("Next issue: `M314-A001`." in packet, str(PACKET_DOC), "M313-E002-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(contract.get("mode") == "m313-e002-validation-consolidation-closeout-matrix-v1", str(CONTRACT_JSON), "M313-E002-CON-01", "mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == "objc3c-cleanup-validation-consolidation-closeout-matrix/m313-e002-v1", str(CONTRACT_JSON), "M313-E002-CON-02", "contract id drifted", failures)
    checks_passed += require(len(contract.get("required_predecessor_summaries", {})) == 14, str(CONTRACT_JSON), "M313-E002-CON-03", "predecessor summary count drifted", failures)
    checks_passed += require(len(contract.get("matrix_rows", [])) == 14, str(CONTRACT_JSON), "M313-E002-CON-04", "matrix row count drifted", failures)
    checks_passed += require(contract.get("pre_closeout_milestone_state", {}).get("milestone_number") == 394, str(CONTRACT_JSON), "M313-E002-CON-05", "milestone number drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M314-A001", str(CONTRACT_JSON), "M313-E002-CON-06", "next issue drifted", failures)

    observational_codes = set(contract.get("observational_predecessor_summaries", []))
    proof_chain: list[dict[str, object]] = []
    for code, rel_path in contract["required_predecessor_summaries"].items():
        path = ROOT / rel_path
        checks_total += 2
        checks_passed += require(path.exists(), rel_path, f"M313-E002-SUM-{code}-EXISTS", f"missing predecessor summary for {code}", failures)
        if path.exists():
            payload = json.loads(path.read_text(encoding="utf-8"))
            proof_chain.append({
                "issue": code,
                "summary_path": rel_path,
                "ok": payload.get("ok"),
                "contract_id": payload.get("contract_id"),
                "observational": code in observational_codes,
            })
            if code in observational_codes:
                checks_passed += require(True, rel_path, f"M313-E002-SUM-{code}-OK", "observational predecessor summary accepted", failures)
            else:
                checks_passed += require(payload.get("ok") is True, rel_path, f"M313-E002-SUM-{code}-OK", f"predecessor summary for {code} must report ok=true", failures)

    topo_path = ROOT / contract["required_topology_summary"]
    checks_total += 2
    checks_passed += require(topo_path.exists(), contract["required_topology_summary"], "M313-E002-TOPO-01", "required topology summary missing", failures)
    topo = json.loads(topo_path.read_text(encoding="utf-8")) if topo_path.exists() else {}
    checks_passed += require(topo.get("ok") is True and topo.get("budget_snapshot", {}).get("active_exception_count") == 0, contract["required_topology_summary"], "M313-E002-TOPO-02", "topology summary not closeout-clean", failures)

    repo_completed = run_command(["gh", "repo", "view", "--json", "nameWithOwner", "-q", ".nameWithOwner"])
    repo_name = repo_completed.stdout.strip() if repo_completed.returncode == 0 else ""
    checks_total += 1
    checks_passed += require(bool(repo_name), "gh repo view", "M313-E002-GH-REPO", "unable to resolve repo nameWithOwner", failures)
    issues_payload = gh_json(["gh", "issue", "list", "--state", "open", "--limit", "1000", "--json", "number,milestone"], "gh issue list", "M313-E002-GH-ISSUES", failures)
    open_issues = issues_payload if isinstance(issues_payload, list) else []
    m313_open = sorted(
        issue["number"]
        for issue in open_issues
        if isinstance(issue, dict)
        and isinstance(issue.get("milestone"), dict)
        and issue["milestone"].get("number") == contract["pre_closeout_milestone_state"]["milestone_number"]
    )
    checks_total += 1
    checks_passed += require(m313_open == contract["pre_closeout_milestone_state"]["expected_open_issue_numbers"], "gh issue list", "M313-E002-GH-M313-OPEN", f"expected pre-closeout M313 open issues {contract['pre_closeout_milestone_state']['expected_open_issue_numbers']}, got {m313_open}", failures)

    matrix_rows = []
    for row in contract["matrix_rows"]:
        source_issue = row["source_issue"]
        summary_path = contract["required_predecessor_summaries"][source_issue]
        matrix_rows.append({
            "row_id": row["row_id"],
            "source_issue": source_issue,
            "status": "supported",
            "evidence": summary_path
        })

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-e002-validation-consolidation-closeout-matrix-cross-lane-integration-sync"' in package, str(PACKAGE_JSON), "M313-E002-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-e002-validation-consolidation-closeout-matrix-cross-lane-integration-sync"' in package, str(PACKAGE_JSON), "M313-E002-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-e002-lane-e-readiness"' in package, str(PACKAGE_JSON), "M313-E002-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": contract["mode"],
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "pre_closeout_open_issue_numbers": m313_open,
        "proof_chain": proof_chain,
        "matrix_rows": matrix_rows,
        "next_issue": "M314-A001",
        "failures": [finding.__dict__ for finding in failures]
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-E002 validation consolidation closeout matrix checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
