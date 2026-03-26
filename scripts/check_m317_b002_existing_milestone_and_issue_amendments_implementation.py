#!/usr/bin/env python3
"""Checker for M317-B002 existing milestone/issue amendments implementation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m317-b002-existing-milestone-issue-amendments-implementation-v1"
CONTRACT_ID = "objc3c-cleanup-existing-milestone-issue-amendments-implementation/m317-b002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-B002" / "existing_milestone_issue_amendments_implementation_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_existing_milestone_and_issue_amendments_implementation_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_b002_existing_milestone_and_issue_amendments_implementation_core_feature_implementation_packet.md"
TARGETS_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_b002_existing_milestone_and_issue_amendments_implementation_core_feature_implementation_targets.json"
PACKAGE_JSON = ROOT / "package.json"
APPLY_SCRIPT = ROOT / "tmp" / "github-publish" / "cleanup_acceleration_program" / "apply_m317_b002_existing_amendments.py"
APPLY_REPORT = ROOT / "tmp" / "reports" / "m317" / "M317-B002" / "existing_milestone_issue_amendments_apply_report.json"
REPO = "doublemover/Slopjective-C"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-github-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M317-B002-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def gh_issue_payload(number: int) -> dict[str, Any]:
    completed = run_command(["gh", "issue", "view", str(number), "--json", "number,title,body,labels"])
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"gh issue view {number} failed")
    payload = json.loads(completed.stdout)
    if not isinstance(payload, dict):
        raise TypeError(f"unexpected payload for issue {number}")
    return payload


def gh_api_payload(path: str) -> dict[str, Any]:
    completed = run_command(["gh", "api", path])
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"gh api {path} failed")
    payload = json.loads(completed.stdout)
    if not isinstance(payload, dict):
        raise TypeError(f"unexpected payload for {path}")
    return payload


def run_github_probes(targets: dict[str, Any], failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    issue_summary: dict[str, Any] = {}

    for issue in targets["issue_targets"]:
        payload = gh_issue_payload(issue["number"])
        body = payload.get("body") or ""
        issue_summary[str(issue["number"])] = {"title": payload["title"], "label_count": len(payload.get("labels", []))}
        checks_total += 2 + len(issue["required_body_snippets"])
        checks_passed += require(len(payload.get("labels", [])) > 0, f"issue#{issue['number']}", "M317-B002-GH-ISSUE-LABELS", "labels missing", failures)
        checks_passed += require("<!-- EXECUTION-ORDER-START -->" in body, f"issue#{issue['number']}", "M317-B002-GH-ISSUE-MARKER", "execution-order marker missing", failures)
        for idx, snippet in enumerate(issue["required_body_snippets"], start=1):
            checks_passed += require(snippet in body, f"issue#{issue['number']}", f"M317-B002-GH-ISSUE-{issue['number']}-{idx:02d}", f"missing body snippet: {snippet}", failures)

    milestone_summary: dict[str, Any] = {}
    for milestone in targets["milestone_targets"]:
        payload = gh_api_payload(f"repos/{REPO}/milestones/{milestone['number']}")
        desc = payload.get("description") or ""
        milestone_summary[str(milestone["number"])] = {"title": payload.get("title", "")}
        checks_total += len(milestone["required_description_snippets"])
        for idx, snippet in enumerate(milestone["required_description_snippets"], start=1):
            checks_passed += require(snippet in desc, f"milestone#{milestone['number']}", f"M317-B002-GH-MILESTONE-{idx:02d}", f"missing description snippet: {snippet}", failures)

    for issue_number in targets["preserved_issue_targets"]:
        payload = gh_issue_payload(issue_number)
        body = payload.get("body") or ""
        checks_total += 1
        checks_passed += require("<!-- EXECUTION-ORDER-START -->" in body, f"issue#{issue_number}", "M317-B002-GH-PRESERVED-MARKER", "execution-order marker missing on preserved issue", failures)

    issue_list = run_command(["gh", "issue", "list", "--state", "open", "--limit", "500", "--json", "number,labels,body"])
    checks_total += 3
    checks_passed += require(issue_list.returncode == 0, "gh issue list", "M317-B002-GH-OPEN-01", issue_list.stderr or issue_list.stdout or "gh issue list failed", failures)
    open_payload = json.loads(issue_list.stdout) if issue_list.returncode == 0 else []
    unlabeled = [item["number"] for item in open_payload if not item.get("labels")]
    missing_execution = [item["number"] for item in open_payload if "<!-- EXECUTION-ORDER-START -->" not in (item.get("body") or "")]
    checks_passed += require(len(unlabeled) == targets["metadata_targets"]["expected_open_issue_unlabeled"], "gh issue list", "M317-B002-GH-OPEN-02", f"unlabeled open issues remain: {unlabeled[:10]}", failures)
    checks_passed += require(len(missing_execution) == targets["metadata_targets"]["expected_open_issue_missing_execution_order"], "gh issue list", "M317-B002-GH-OPEN-03", f"open issues missing execution markers: {missing_execution[:10]}", failures)

    return checks_total, checks_passed, {
        "issues": issue_summary,
        "milestones": milestone_summary,
        "open_issue_count": len(open_payload),
        "unlabeled_open_issues": unlabeled,
        "open_issues_missing_execution_order": missing_execution,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M317-B002-EXP-01", "# M317 Existing Milestone and Issue Amendments Implementation Core Feature Implementation Expectations (B002)"),
            SnippetCheck("M317-B002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M317-B002-EXP-03", "`#7529-#7538`"),
            SnippetCheck("M317-B002-EXP-04", "`M313-M318`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M317-B002-PKT-01", "# M317-B002 Packet: Existing milestone and issue amendments implementation - Core feature implementation"),
            SnippetCheck("M317-B002-PKT-02", "m288_milestone_boundary_preservation"),
            SnippetCheck("M317-B002-PKT-03", "Next issue: `M317-B003`."),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M317-B002-PKG-01", '"check:objc3c:m317-b002-existing-milestone-and-issue-amendments-implementation"'),
            SnippetCheck("M317-B002-PKG-02", '"test:tooling:m317-b002-existing-milestone-and-issue-amendments-implementation"'),
            SnippetCheck("M317-B002-PKG-03", '"check:objc3c:m317-b002-lane-b-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    targets = load_json(TARGETS_JSON)
    checks_total += 8
    checks_passed += require(targets.get("mode") == MODE, display_path(TARGETS_JSON), "M317-B002-TGT-01", "mode drifted", failures)
    checks_passed += require(targets.get("contract_id") == CONTRACT_ID, display_path(TARGETS_JSON), "M317-B002-TGT-02", "contract id drifted", failures)
    checks_passed += require(len(targets.get("milestone_targets", [])) == 1, display_path(TARGETS_JSON), "M317-B002-TGT-03", "milestone target count drifted", failures)
    checks_passed += require(len(targets.get("issue_targets", [])) == 10, display_path(TARGETS_JSON), "M317-B002-TGT-04", "issue target count drifted", failures)
    checks_passed += require(targets["milestone_targets"][0]["number"] == 373, display_path(TARGETS_JSON), "M317-B002-TGT-05", "M288 milestone target drifted", failures)
    checks_passed += require(set(targets.get("preserved_issue_targets", [])) == {7399, 7421, 7425, 7428, 7434, 7438, 7441}, display_path(TARGETS_JSON), "M317-B002-TGT-06", "preserved issue set drifted", failures)
    checks_passed += require(targets.get("metadata_targets", {}).get("expected_open_issue_unlabeled") == 0, display_path(TARGETS_JSON), "M317-B002-TGT-07", "open unlabeled target drifted", failures)
    checks_passed += require(targets.get("metadata_targets", {}).get("expected_open_issue_missing_execution_order") == 0, display_path(TARGETS_JSON), "M317-B002-TGT-08", "missing execution target drifted", failures)

    checks_total += 2
    checks_passed += require(APPLY_SCRIPT.exists(), display_path(APPLY_SCRIPT), "M317-B002-APL-01", "apply script missing", failures)
    checks_passed += require(APPLY_REPORT.exists(), display_path(APPLY_REPORT), "M317-B002-APL-02", "apply report missing", failures)

    github_summary: dict[str, object] = {"skipped": True}
    github_executed = False
    if not args.skip_github_probes:
        github_executed = True
        gh_total, gh_passed, github_summary = run_github_probes(targets, failures)
        checks_total += gh_total
        checks_passed += gh_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "github_probes_executed": github_executed,
        "failures": [finding.__dict__ for finding in failures],
        "target_issue_numbers": [item["number"] for item in targets.get("issue_targets", [])],
        "target_milestone_numbers": [item["number"] for item in targets.get("milestone_targets", [])],
        "github_summary": github_summary,
        "next_issue": "M317-B003",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-B002 existing amendment implementation checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
