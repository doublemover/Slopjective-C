#!/usr/bin/env python3
"""Checker for M317-B003 future milestone dependency rewrites for post-M292 work."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m317-b003-future-milestone-dependency-rewrites-v1"
CONTRACT_ID = "objc3c-cleanup-future-milestone-dependency-rewrites/m317-b003-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-B003" / "future_milestone_dependency_rewrites_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_future_milestone_dependency_rewrites_for_post_m292_work_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_b003_future_milestone_dependency_rewrites_for_post_m292_work_edge_case_and_compatibility_completion_packet.md"
TARGETS_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_b003_future_milestone_dependency_rewrites_for_post_m292_work_edge_case_and_compatibility_completion_targets.json"
PACKAGE_JSON = ROOT / "package.json"
APPLY_SCRIPT = ROOT / "tmp" / "github-publish" / "cleanup_acceleration_program" / "apply_m317_b003_future_dependency_rewrites.py"
APPLY_REPORT = ROOT / "tmp" / "reports" / "m317" / "M317-B003" / "future_milestone_dependency_rewrites_apply_report.json"


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
        failures.append(Finding(display_path(path), "M317-B003-MISSING", f"missing artifact: {display_path(path)}"))
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


def gh_api_payload(path: str) -> dict[str, Any]:
    completed = run_command(["gh", "api", path])
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"gh api {path} failed")
    payload = json.loads(completed.stdout)
    if not isinstance(payload, dict):
        raise TypeError(f"unexpected payload for {path}")
    return payload


def gh_issue_list_for_milestone(title: str) -> list[dict[str, Any]]:
    completed = run_command(
        ["gh", "issue", "list", "--state", "open", "--milestone", title, "--limit", "100", "--json", "number,title,body,labels"]
    )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"gh issue list for milestone {title!r} failed")
    payload = json.loads(completed.stdout)
    if not isinstance(payload, list):
        raise TypeError(f"unexpected issue list payload for milestone {title}")
    return payload


def run_github_probes(targets: dict[str, Any], failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    milestone_summary: dict[str, Any] = {}

    for milestone in targets["milestone_targets"]:
        milestone_payload = gh_api_payload(f"repos/doublemover/Slopjective-C/milestones/{milestone['number']}")
        desc = milestone_payload.get("description") or ""
        checks_total += len(milestone["required_description_snippets"])
        for idx, snippet in enumerate(milestone["required_description_snippets"], start=1):
            checks_passed += require(
                snippet in desc,
                f"milestone#{milestone['number']}",
                f"M317-B003-GH-MILESTONE-{milestone['number']}-{idx:02d}",
                f"missing description snippet: {snippet}",
                failures,
            )

        issues = gh_issue_list_for_milestone(milestone["title"])
        milestone_summary[str(milestone["number"])] = {
            "title": milestone["title"],
            "family": milestone["family"],
            "open_issue_count": len(issues),
        }
        checks_total += 1
        checks_passed += require(
            len(issues) == milestone["expected_open_issue_count"],
            f"milestone#{milestone['number']}",
            f"M317-B003-GH-COUNT-{milestone['number']}",
            f"expected {milestone['expected_open_issue_count']} open issues, found {len(issues)}",
            failures,
        )

        for issue in issues:
            body = issue.get("body") or ""
            checks_total += 2 + len(milestone["required_issue_body_snippets"])
            checks_passed += require(
                len(issue.get("labels") or []) > 0,
                f"issue#{issue['number']}",
                "M317-B003-GH-ISSUE-LABELS",
                "labels missing",
                failures,
            )
            checks_passed += require(
                "<!-- EXECUTION-ORDER-START -->" in body,
                f"issue#{issue['number']}",
                "M317-B003-GH-ISSUE-MARKER",
                "execution-order marker missing",
                failures,
            )
            for idx, snippet in enumerate(milestone["required_issue_body_snippets"], start=1):
                checks_passed += require(
                    snippet in body,
                    f"issue#{issue['number']}",
                    f"M317-B003-GH-ISSUE-{issue['number']}-{idx:02d}",
                    f"missing body snippet: {snippet}",
                    failures,
                )

    issue_list = run_command(["gh", "issue", "list", "--state", "open", "--limit", "500", "--json", "number,labels,body"])
    checks_total += 3
    checks_passed += require(
        issue_list.returncode == 0,
        "gh issue list",
        "M317-B003-GH-OPEN-01",
        issue_list.stderr or issue_list.stdout or "gh issue list failed",
        failures,
    )
    open_payload = json.loads(issue_list.stdout) if issue_list.returncode == 0 else []
    unlabeled = [item["number"] for item in open_payload if not item.get("labels")]
    missing_execution = [item["number"] for item in open_payload if "<!-- EXECUTION-ORDER-START -->" not in (item.get("body") or "")]
    checks_passed += require(
        len(unlabeled) == targets["metadata_targets"]["expected_open_issue_unlabeled"],
        "gh issue list",
        "M317-B003-GH-OPEN-02",
        f"unlabeled open issues remain: {unlabeled[:10]}",
        failures,
    )
    checks_passed += require(
        len(missing_execution) == targets["metadata_targets"]["expected_open_issue_missing_execution_order"],
        "gh issue list",
        "M317-B003-GH-OPEN-03",
        f"open issues missing execution markers: {missing_execution[:10]}",
        failures,
    )

    return checks_total, checks_passed, {
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
            SnippetCheck("M317-B003-EXP-01", "# M317 Future Milestone Dependency Rewrites For Post-M292 Work Edge Case And Compatibility Completion Expectations (B003)"),
            SnippetCheck("M317-B003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M317-B003-EXP-03", "`M293-M308`"),
            SnippetCheck("M317-B003-EXP-04", "`M313`"),
            SnippetCheck("M317-B003-EXP-05", "`M316`")
        ],
        PACKET_DOC: [
            SnippetCheck("M317-B003-PKT-01", "# M317-B003 Packet: Future milestone dependency rewrites for post-M292 work - Edge case and compatibility completion"),
            SnippetCheck("M317-B003-PKT-02", "Corrective dependencies consumed"),
            SnippetCheck("M317-B003-PKT-03", "Next issue: `M317-C001`."),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M317-B003-PKG-01", '"check:objc3c:m317-b003-future-milestone-dependency-rewrites-for-post-m292-work"'),
            SnippetCheck("M317-B003-PKG-02", '"test:tooling:m317-b003-future-milestone-dependency-rewrites-for-post-m292-work"'),
            SnippetCheck("M317-B003-PKG-03", '"check:objc3c:m317-b003-lane-b-readiness"'),
        ],
    }
    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    targets = load_json(TARGETS_JSON)
    checks_total += 8
    checks_passed += require(targets.get("mode") == MODE, display_path(TARGETS_JSON), "M317-B003-TGT-01", "mode drifted", failures)
    checks_passed += require(targets.get("contract_id") == CONTRACT_ID, display_path(TARGETS_JSON), "M317-B003-TGT-02", "contract id drifted", failures)
    milestone_targets = targets.get("milestone_targets", [])
    checks_passed += require(len(milestone_targets) == 16, display_path(TARGETS_JSON), "M317-B003-TGT-03", "milestone target count drifted", failures)
    checks_passed += require(sum(item.get("expected_open_issue_count", 0) for item in milestone_targets) == 180, display_path(TARGETS_JSON), "M317-B003-TGT-04", "targeted issue count drifted", failures)
    checks_passed += require({item["number"] for item in milestone_targets} == set(range(378, 394)), display_path(TARGETS_JSON), "M317-B003-TGT-05", "targeted milestone range drifted", failures)
    checks_passed += require({item["family"] for item in milestone_targets} == {"performance", "distribution", "conformance", "stdlib"}, display_path(TARGETS_JSON), "M317-B003-TGT-06", "family set drifted", failures)
    checks_passed += require(targets.get("metadata_targets", {}).get("expected_open_issue_unlabeled") == 0, display_path(TARGETS_JSON), "M317-B003-TGT-07", "open unlabeled target drifted", failures)
    checks_passed += require(targets.get("metadata_targets", {}).get("expected_open_issue_missing_execution_order") == 0, display_path(TARGETS_JSON), "M317-B003-TGT-08", "missing execution target drifted", failures)

    checks_total += 2
    checks_passed += require(APPLY_SCRIPT.exists(), display_path(APPLY_SCRIPT), "M317-B003-APL-01", "apply script missing", failures)
    checks_passed += require(APPLY_REPORT.exists(), display_path(APPLY_REPORT), "M317-B003-APL-02", "apply report missing", failures)

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
        "target_milestone_numbers": [item["number"] for item in milestone_targets],
        "target_total_issue_count": sum(item["expected_open_issue_count"] for item in milestone_targets),
        "github_summary": github_summary,
        "next_issue": "M317-C001",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-B003 future dependency rewrite checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
