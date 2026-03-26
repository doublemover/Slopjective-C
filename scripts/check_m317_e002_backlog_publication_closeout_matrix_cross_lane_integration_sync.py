#!/usr/bin/env python3
"""Checker for M317-E002 backlog publication closeout matrix."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-E002" / "backlog_publication_closeout_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_backlog_publication_closeout_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_e002_backlog_publication_closeout_matrix_cross_lane_integration_sync_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_e002_backlog_publication_closeout_matrix_cross_lane_integration_sync_contract.json"
PACKAGE_JSON = ROOT / "package.json"
ROADMAP_TEMPLATE = ROOT / ".github" / "ISSUE_TEMPLATE" / "roadmap_execution.yml"
CONFORMANCE_TEMPLATE = ROOT / ".github" / "ISSUE_TEMPLATE" / "conformance_execution.yml"
SEED_GENERATOR = ROOT / "tmp" / "planning" / "cleanup_acceleration_program" / "generate_cleanup_acceleration_program.py"

M317_MILESTONE_NUMBER = 398
M288_MILESTONE_NUMBER = 373
FUTURE_MILESTONE_NUMBERS = tuple(range(378, 394))
EXPECTED_M317_OPEN_ISSUES = [7834]
EXPECTED_M288_OPEN_ISSUES = list(range(7529, 7539))
EXPECTED_FUTURE_ISSUE_COUNT = 180
UPSTREAM_HANDOFF_ISSUE = "M317-E001"
NEXT_ISSUE = "M313-A001"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


@dataclass(frozen=True)
class MilestoneInfo:
    number: int
    title: str
    description: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def milestone_map(repo: str, numbers: Sequence[int], failures: list[Finding]) -> dict[int, MilestoneInfo]:
    result: dict[int, MilestoneInfo] = {}
    for number in numbers:
        payload = gh_json(["gh", "api", f"repos/{repo}/milestones/{number}"], f"github:milestone:{number}", f"M317-E002-GH-M{number}", failures)
        if not isinstance(payload, dict):
            continue
        result[number] = MilestoneInfo(number=number, title=str(payload.get("title", "")), description=str(payload.get("description", "")))
    return result


def has_label(issue: dict[str, Any], label_name: str) -> bool:
    return any(label.get("name") == label_name for label in issue.get("labels", []))


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = json.loads(read_text(CONTRACT_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 5
    checks_passed += require("Contract ID: `objc3c-cleanup-backlog-publication-closeout-matrix/m317-e002-v1`" in expectations, str(EXPECTATIONS_DOC), "M317-E002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Pre-closeout milestone state is reduced to the final closeout issue itself: `#7834`." in expectations, str(EXPECTATIONS_DOC), "M317-E002-EXP-02", "expectations missing final-open-issue requirement", failures)
    checks_passed += require("matrix rows for `M317-A001` through `M317-E001`" in expectations, str(EXPECTATIONS_DOC), "M317-E002-EXP-03", "expectations missing matrix-row requirement", failures)
    checks_passed += require("pre-closeout milestone-open-issue condition" in packet, str(PACKET_DOC), "M317-E002-PKT-01", "packet missing pre-closeout state focus", failures)
    checks_passed += require("Next issue: `M313-A001`." in packet, str(PACKET_DOC), "M317-E002-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(contract.get("mode") == "m317-e002-backlog-publication-closeout-matrix-v1", str(CONTRACT_JSON), "M317-E002-CON-01", "mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == "objc3c-cleanup-backlog-publication-closeout-matrix/m317-e002-v1", str(CONTRACT_JSON), "M317-E002-CON-02", "contract id drifted", failures)
    checks_passed += require(set(contract.get("required_predecessor_summaries", {}).keys()) == {"M317-A001", "M317-A002", "M317-B001", "M317-B002", "M317-B003", "M317-C001", "M317-C002", "M317-D001", "M317-E001"}, str(CONTRACT_JSON), "M317-E002-CON-03", "predecessor summary set drifted", failures)
    checks_passed += require(set(contract.get("required_live_checks", [])) == {"open_roadmap_issue_label_completeness", "open_roadmap_issue_execution_marker_completeness", "m288_boundary_amendments_preserved", "m293_m308_dependency_rewrites_preserved", "template_generator_contract_alignment_preserved"}, str(CONTRACT_JSON), "M317-E002-CON-04", "required live checks drifted", failures)
    checks_passed += require(contract.get("pre_closeout_milestone_state", {}).get("milestone_number") == M317_MILESTONE_NUMBER, str(CONTRACT_JSON), "M317-E002-CON-05", "milestone number drifted", failures)
    checks_passed += require(contract.get("pre_closeout_milestone_state", {}).get("expected_open_issue_numbers") == EXPECTED_M317_OPEN_ISSUES, str(CONTRACT_JSON), "M317-E002-CON-06", "expected open issue numbers drifted", failures)

    predecessor_chain: list[dict[str, Any]] = []
    for code, rel_path in contract["required_predecessor_summaries"].items():
        path = ROOT / rel_path
        artifact = display_path(path)
        checks_total += 1
        checks_passed += require(path.exists(), artifact, f"M317-E002-SUM-{code}-EXISTS", f"missing predecessor summary for {code}", failures)
        if not path.exists():
            continue
        payload = json.loads(path.read_text(encoding="utf-8"))
        predecessor_chain.append({
            "issue": code,
            "summary_path": artifact,
            "ok": payload.get("ok"),
            "checks_total": payload.get("checks_total"),
            "checks_passed": payload.get("checks_passed"),
            "contract_id": payload.get("contract_id"),
        })
        checks_total += 1
        checks_passed += require(payload.get("ok") is True, artifact, f"M317-E002-SUM-{code}-OK", f"predecessor summary for {code} must report ok=true", failures)

    checks_total += 4
    checks_passed += require(predecessor_chain and predecessor_chain[0]["issue"] == "M317-A001", str(CONTRACT_JSON), "M317-E002-CHAIN-01", "proof chain must start at M317-A001", failures)
    checks_passed += require(predecessor_chain and predecessor_chain[-1]["issue"] == "M317-E001", str(CONTRACT_JSON), "M317-E002-CHAIN-02", "proof chain must end at M317-E001", failures)
    checks_passed += require(UPSTREAM_HANDOFF_ISSUE == "M317-E001", str(CONTRACT_JSON), "M317-E002-CHAIN-03", "upstream handoff issue drifted", failures)
    checks_passed += require(contract.get("next_issue") == NEXT_ISSUE, str(CONTRACT_JSON), "M317-E002-CHAIN-04", "next issue drifted", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m317-e002-backlog-publication-closeout-matrix-cross-lane-integration-sync"' in package, str(PACKAGE_JSON), "M317-E002-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m317-e002-backlog-publication-closeout-matrix-cross-lane-integration-sync"' in package, str(PACKAGE_JSON), "M317-E002-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m317-e002-lane-e-readiness"' in package, str(PACKAGE_JSON), "M317-E002-PKG-03", "package missing readiness script", failures)

    template_text = read_text(ROADMAP_TEMPLATE)
    conformance_text = read_text(CONFORMANCE_TEMPLATE)
    generator_text = read_text(SEED_GENERATOR)
    checks_total += 6
    checks_passed += require("validation_posture" in template_text, display_path(ROADMAP_TEMPLATE), "M317-E002-TPL-01", "roadmap template missing validation posture field", failures)
    checks_passed += require("## Required deliverables" not in template_text, display_path(ROADMAP_TEMPLATE), "M317-E002-TPL-02", "roadmap template still contains Required deliverables boilerplate", failures)
    checks_passed += require("validation_posture" in conformance_text, display_path(CONFORMANCE_TEMPLATE), "M317-E002-TPL-03", "conformance template missing validation posture field", failures)
    checks_passed += require("## Required deliverables" not in conformance_text, display_path(CONFORMANCE_TEMPLATE), "M317-E002-TPL-04", "conformance template still contains Required deliverables boilerplate", failures)
    checks_passed += require("## Validation posture" in generator_text, display_path(SEED_GENERATOR), "M317-E002-TPL-05", "seed generator missing validation posture section", failures)
    checks_passed += require("## Required deliverables" not in generator_text, display_path(SEED_GENERATOR), "M317-E002-TPL-06", "seed generator still emits Required deliverables boilerplate", failures)

    repo = run_command(["gh", "repo", "view", "--json", "nameWithOwner", "-q", ".nameWithOwner"])
    repo_name = repo.stdout.strip() if repo.returncode == 0 else ""
    checks_total += 1
    checks_passed += require(bool(repo_name), "gh repo view", "M317-E002-GH-REPO", "unable to resolve repo nameWithOwner", failures)

    issues_payload = gh_json(["gh", "issue", "list", "--state", "open", "--limit", "1000", "--json", "number,labels,body,milestone"], "gh issue list", "M317-E002-GH-ISSUES", failures)
    open_issues = issues_payload if isinstance(issues_payload, list) else []
    all_open_issue_numbers = sorted(issue.get("number") for issue in open_issues if isinstance(issue, dict) and isinstance(issue.get("number"), int))

    checks_total += 2
    unlabeled_open_issues = sorted(issue["number"] for issue in open_issues if isinstance(issue, dict) and not issue.get("labels"))
    checks_passed += require(not unlabeled_open_issues, "gh issue list", "M317-E002-GH-LABELS", "found unlabeled open issues", failures)
    open_roadmap_issues = [issue for issue in open_issues if isinstance(issue, dict) and has_label(issue, "type:roadmap")]
    missing_execution_order = sorted(issue["number"] for issue in open_roadmap_issues if "<!-- EXECUTION-ORDER-START -->" not in issue.get("body", "") or "<!-- EXECUTION-ORDER-END -->" not in issue.get("body", ""))
    checks_passed += require(not missing_execution_order, "gh issue list", "M317-E002-GH-EXEC", "found open roadmap issues missing execution-order markers", failures)

    milestone_398_open = sorted(issue["number"] for issue in open_issues if isinstance(issue, dict) and isinstance(issue.get("milestone"), dict) and issue["milestone"].get("number") == M317_MILESTONE_NUMBER)
    checks_total += 1
    checks_passed += require(milestone_398_open == EXPECTED_M317_OPEN_ISSUES, "gh issue list", "M317-E002-GH-M317-OPEN", f"expected M317 open issues {EXPECTED_M317_OPEN_ISSUES}, got {milestone_398_open}", failures)

    milestone_373_open = sorted(issue["number"] for issue in open_issues if isinstance(issue, dict) and isinstance(issue.get("milestone"), dict) and issue["milestone"].get("number") == M288_MILESTONE_NUMBER)
    checks_total += 2
    checks_passed += require(milestone_373_open == EXPECTED_M288_OPEN_ISSUES, "gh issue list", "M317-E002-GH-M288-LIST", "M288 open-issue set drifted", failures)
    m288_bodies_ok = all(
        "## Boundary note" in issue.get("body", "")
        and any(token in issue.get("body", "") for token in ["M313-M318", "`M313`", "`M314`", "`M315`", "`M318`"])
        for issue in open_issues
        if isinstance(issue, dict) and isinstance(issue.get("milestone"), dict) and issue["milestone"].get("number") == M288_MILESTONE_NUMBER
    )
    checks_passed += require(m288_bodies_ok, "gh issue list", "M317-E002-GH-M288-BODIES", "M288 issue boundary notes drifted", failures)

    future_open_issues = [issue for issue in open_issues if isinstance(issue, dict) and isinstance(issue.get("milestone"), dict) and issue["milestone"].get("number") in FUTURE_MILESTONE_NUMBERS]
    checks_total += 2
    checks_passed += require(len(future_open_issues) == EXPECTED_FUTURE_ISSUE_COUNT, "gh issue list", "M317-E002-GH-FUTURE-COUNT", f"expected {EXPECTED_FUTURE_ISSUE_COUNT} post-cleanup issues, got {len(future_open_issues)}", failures)
    future_issue_rewrites_ok = all("## Post-cleanup dependency rewrite" in issue.get("body", "") for issue in future_open_issues)
    checks_passed += require(future_issue_rewrites_ok, "gh issue list", "M317-E002-GH-FUTURE-BODIES", "future issue dependency rewrites drifted", failures)

    checked_milestones = [M288_MILESTONE_NUMBER, *FUTURE_MILESTONE_NUMBERS, M317_MILESTONE_NUMBER]
    milestones = milestone_map(repo_name, checked_milestones, failures) if repo_name else {}
    checks_total += 1
    checks_passed += require(M288_MILESTONE_NUMBER in milestones and "M313-M318" in milestones[M288_MILESTONE_NUMBER].description, f"github:milestone:{M288_MILESTONE_NUMBER}", "M317-E002-GH-M288-DESC", "M288 milestone description lost corrective-program boundary note", failures)

    checks_total += len(FUTURE_MILESTONE_NUMBERS)
    for number in FUTURE_MILESTONE_NUMBERS:
        info = milestones.get(number)
        checks_passed += require(info is not None and "Corrective dependencies consumed:" in info.description and "M313" in info.description and "M314" in info.description and "M315" in info.description and "M318" in info.description, f"github:milestone:{number}", f"M317-E002-GH-FUTURE-M{number}", f"future milestone {number} lost corrective dependency rewrite block", failures)

    checks_total += 1
    checks_passed += require(M317_MILESTONE_NUMBER in milestones and "Execution order:" in milestones[M317_MILESTONE_NUMBER].description and "M313" in milestones[M317_MILESTONE_NUMBER].description, f"github:milestone:{M317_MILESTONE_NUMBER}", "M317-E002-GH-M317-DESC", "M317 milestone description lost execution-order block", failures)

    matrix_rows = []
    for entry in predecessor_chain:
        matrix_rows.append({
            "row_id": entry["issue"].lower().replace("-", "_"),
            "source_issue": entry["issue"],
            "status": "supported",
            "evidence": entry["summary_path"],
            "model": f"{entry['issue']} closeout evidence remains truthful in the simplified backlog-publication program",
        })

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": contract["mode"],
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "github_probes_executed": True,
        "upstream_handoff_issue": UPSTREAM_HANDOFF_ISSUE,
        "next_issue": NEXT_ISSUE,
        "checked_milestones": checked_milestones,
        "checked_issues": len(all_open_issue_numbers),
        "unlabeled_open_issues": unlabeled_open_issues,
        "open_issues_missing_execution_order": missing_execution_order,
        "pre_closeout_open_issue_numbers": milestone_398_open,
        "proof_chain": predecessor_chain,
        "matrix_rows": matrix_rows,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-E002 backlog publication closeout matrix checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
