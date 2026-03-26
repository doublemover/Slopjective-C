#!/usr/bin/env python3
"""Checker for M317-A002 current GitHub amendment/supersession map."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m317-a002-current-github-amendment-supersession-map-v1"
CONTRACT_ID = "objc3c-cleanup-current-github-amendment-supersession-map/m317-a002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-A002" / "current_github_issue_amendment_and_supersession_map_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_current_github_issue_amendment_and_supersession_map_source_completion_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_a002_current_github_issue_amendment_and_supersession_map_source_completion_packet.md"
MAP_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_a002_current_github_issue_amendment_and_supersession_map_source_completion_map.json"
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_a001_backlog_overlap_and_correction_inventory_contract_and_architecture_freeze_inventory.json"
GUIDANCE_DOC = ROOT / "tmp" / "planning" / "cleanup_acceleration_program" / "github_issue_alteration_guidance.md"
PACKAGE_JSON = ROOT / "package.json"
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
        failures.append(Finding(display_path(path), "M317-A002-MISSING", f"missing artifact: {display_path(path)}"))
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
    completed = run_command(["gh", "issue", "view", str(number), "--json", "number,title,body,labels,milestone"])
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


def run_github_probes(mapping: dict[str, Any], failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    issue_summaries: dict[str, Any] = {}

    for entry in mapping.get("current_issue_amendments", []):
        issue_number = entry["issue"]
        payload = gh_issue_payload(issue_number)
        issue_summaries[str(issue_number)] = {"title": payload["title"], "label_count": len(payload.get("labels", []))}
        body = payload.get("body") or ""
        checks_total += 3 + len(entry.get("required_body_snippets", []))
        checks_passed += require(payload["title"] == entry["required_title"], f"issue#{issue_number}", "M317-A002-GH-ISSUE-TITLE", f"title drifted: {payload['title']}", failures)
        checks_passed += require("<!-- EXECUTION-ORDER-START -->" in body, f"issue#{issue_number}", "M317-A002-GH-ISSUE-MARKER", "execution-order marker missing", failures)
        checks_passed += require(len(payload.get("labels", [])) > 0, f"issue#{issue_number}", "M317-A002-GH-ISSUE-LABELS", "labels missing", failures)
        for idx, snippet in enumerate(entry.get("required_body_snippets", []), start=1):
            checks_passed += require(snippet in body, f"issue#{issue_number}", f"M317-A002-GH-ISSUE-{issue_number}-{idx:02d}", f"missing body snippet: {snippet}", failures)

    milestone_summaries: dict[str, Any] = {}
    for entry in mapping.get("milestone_boundary_entries", []):
        milestone_number = entry["number"]
        payload = gh_api_payload(f"repos/{REPO}/milestones/{milestone_number}")
        desc = payload.get("description") or ""
        milestone_summaries[entry["code"]] = {"title": payload.get("title", "")}
        checks_total += 3
        checks_passed += require(payload.get("title", "").startswith(entry["code"]), f"milestone#{milestone_number}", "M317-A002-GH-MILESTONE-TITLE", f"milestone title drifted: {payload.get('title', '')}", failures)
        checks_passed += require("<!-- EXECUTION-ORDER-START -->" in desc, f"milestone#{milestone_number}", "M317-A002-GH-MILESTONE-MARKER", "execution-order marker missing", failures)
        checks_passed += require(entry["required_description_snippet"] in desc, f"milestone#{milestone_number}", "M317-A002-GH-MILESTONE-SNIPPET", f"missing description snippet: {entry['required_description_snippet']}", failures)

    followup = mapping.get("followup_issue", {})
    followup_issue = gh_issue_payload(followup["issue"])
    checks_total += 3 + len(followup.get("required_body_snippets", []))
    checks_passed += require(followup_issue["title"] == followup["required_title"], f"issue#{followup['issue']}", "M317-A002-GH-FOLLOWUP-TITLE", f"title drifted: {followup_issue['title']}", failures)
    checks_passed += require("<!-- EXECUTION-ORDER-START -->" in (followup_issue.get("body") or ""), f"issue#{followup['issue']}", "M317-A002-GH-FOLLOWUP-MARKER", "execution-order marker missing", failures)
    checks_passed += require(len(followup_issue.get("labels", [])) > 0, f"issue#{followup['issue']}", "M317-A002-GH-FOLLOWUP-LABELS", "labels missing", failures)
    for idx, snippet in enumerate(followup.get("required_body_snippets", []), start=1):
        checks_passed += require(snippet in (followup_issue.get("body") or ""), f"issue#{followup['issue']}", f"M317-A002-GH-FOLLOWUP-{idx:02d}", f"missing body snippet: {snippet}", failures)

    issue_list = run_command(["gh", "issue", "list", "--state", "open", "--limit", "500", "--json", "number,labels,body"])
    checks_total += 3
    checks_passed += require(issue_list.returncode == 0, "gh issue list", "M317-A002-GH-OPEN-01", issue_list.stderr or issue_list.stdout or "gh issue list failed", failures)
    open_payload = json.loads(issue_list.stdout) if issue_list.returncode == 0 else []
    unlabeled = [item["number"] for item in open_payload if not item.get("labels")]
    missing_execution = [item["number"] for item in open_payload if "<!-- EXECUTION-ORDER-START -->" not in (item.get("body") or "")]
    checks_passed += require(len(unlabeled) == mapping["metadata_targets"]["expected_open_issue_unlabeled"], "gh issue list", "M317-A002-GH-OPEN-02", f"unlabeled open issues remain: {unlabeled[:10]}", failures)
    checks_passed += require(len(missing_execution) == mapping["metadata_targets"]["expected_open_issue_missing_execution_order"], "gh issue list", "M317-A002-GH-OPEN-03", f"open issues missing execution markers: {missing_execution[:10]}", failures)

    return checks_total, checks_passed, {
        "issues": issue_summaries,
        "milestones": milestone_summaries,
        "followup_label_count": len(followup_issue.get("labels", [])),
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
            SnippetCheck("M317-A002-EXP-01", "# M317 Current GitHub Issue Amendment and Supersession Map Source Completion Expectations (A002)"),
            SnippetCheck("M317-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M317-A002-EXP-03", "`#7399`"),
            SnippetCheck("M317-A002-EXP-04", "`M293-M304`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M317-A002-PKT-01", "# M317-A002 Packet: Current GitHub issue amendment and supersession map - Source completion"),
            SnippetCheck("M317-A002-PKT-02", "current_issue_amendments"),
            SnippetCheck("M317-A002-PKT-03", "metadata_targets"),
            SnippetCheck("M317-A002-PKT-04", "Next issue: `M317-B001`."),
        ],
        GUIDANCE_DOC: [
            SnippetCheck("M317-A002-GUI-01", "`#7399`"),
            SnippetCheck("M317-A002-GUI-02", "`#7421`"),
            SnippetCheck("M317-A002-GUI-03", "`#7441`"),
            SnippetCheck("M317-A002-GUI-04", "### M293-M304 future work interaction"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M317-A002-PKG-01", '"check:objc3c:m317-a002-current-github-issue-amendment-and-supersession-map-source-completion"'),
            SnippetCheck("M317-A002-PKG-02", '"test:tooling:m317-a002-current-github-issue-amendment-and-supersession-map-source-completion"'),
            SnippetCheck("M317-A002-PKG-03", '"check:objc3c:m317-a002-lane-a-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    mapping = load_json(MAP_JSON)
    inventory = load_json(INVENTORY_JSON)
    checks_total += 10
    checks_passed += require(mapping.get("mode") == MODE, display_path(MAP_JSON), "M317-A002-MAP-01", "mode drifted", failures)
    checks_passed += require(mapping.get("contract_id") == CONTRACT_ID, display_path(MAP_JSON), "M317-A002-MAP-02", "contract id drifted", failures)
    checks_passed += require(mapping.get("dependency_contract_id") == inventory.get("contract_id"), display_path(MAP_JSON), "M317-A002-MAP-03", "dependency contract mismatch", failures)
    current_issue_amendments = mapping.get("current_issue_amendments", [])
    checks_passed += require(isinstance(current_issue_amendments, list) and len(current_issue_amendments) == 7, display_path(MAP_JSON), "M317-A002-MAP-04", "current amendment count drifted", failures)
    amendment_issue_numbers = {item["issue"] for item in current_issue_amendments}
    checks_passed += require(amendment_issue_numbers == {7399, 7421, 7425, 7428, 7434, 7438, 7441}, display_path(MAP_JSON), "M317-A002-MAP-05", "amendment issue set drifted", failures)
    milestone_entries = mapping.get("milestone_boundary_entries", [])
    checks_passed += require(isinstance(milestone_entries, list) and len(milestone_entries) == 14, display_path(MAP_JSON), "M317-A002-MAP-06", "milestone boundary count drifted", failures)
    milestone_codes = {item["code"] for item in milestone_entries}
    checks_passed += require({"M288", "M293", "M304", "M317"}.issubset(milestone_codes), display_path(MAP_JSON), "M317-A002-MAP-07", "milestone boundary set drifted", failures)
    followup = mapping.get("followup_issue", {})
    checks_passed += require(followup.get("issue") == 7764, display_path(MAP_JSON), "M317-A002-MAP-08", "followup issue drifted", failures)
    metadata_targets = mapping.get("metadata_targets", {})
    checks_passed += require(metadata_targets.get("expected_open_issue_unlabeled") == 0, display_path(MAP_JSON), "M317-A002-MAP-09", "open unlabeled target drifted", failures)
    checks_passed += require(metadata_targets.get("expected_open_issue_missing_execution_order") == 0, display_path(MAP_JSON), "M317-A002-MAP-10", "missing execution target drifted", failures)

    github_summary: dict[str, object] = {"skipped": True}
    github_executed = False
    if not args.skip_github_probes:
        github_executed = True
        gh_total, gh_passed, github_summary = run_github_probes(mapping, failures)
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
        "amendment_issue_numbers": sorted(amendment_issue_numbers),
        "milestone_boundary_codes": sorted(milestone_codes),
        "github_summary": github_summary,
        "next_issue": "M317-B001",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-A002 current amendment/supersession map checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
