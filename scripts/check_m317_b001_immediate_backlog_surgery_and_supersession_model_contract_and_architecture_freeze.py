#!/usr/bin/env python3
"""Checker for M317-B001 immediate backlog surgery model."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m317-b001-immediate-backlog-surgery-supersession-model-v1"
CONTRACT_ID = "objc3c-cleanup-immediate-backlog-surgery-supersession-model/m317-b001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-B001" / "immediate_backlog_surgery_and_supersession_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_immediate_backlog_surgery_and_supersession_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_b001_immediate_backlog_surgery_and_supersession_model_contract_and_architecture_freeze_packet.md"
MODEL_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_b001_immediate_backlog_surgery_and_supersession_model_contract_and_architecture_freeze_model.json"
MAP_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_a002_current_github_issue_amendment_and_supersession_map_source_completion_map.json"
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
        failures.append(Finding(display_path(path), "M317-B001-MISSING", f"missing artifact: {display_path(path)}"))
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


def run_github_probes(model: dict[str, Any], failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    issue_payloads = {}
    for issue_number in [7399, 7421, 7425, 7428, 7434, 7438, 7441, 7764]:
        payload = gh_issue_payload(issue_number)
        issue_payloads[str(issue_number)] = {"title": payload["title"], "label_count": len(payload.get("labels", []))}
        body = payload.get("body") or ""
        checks_total += 2
        checks_passed += require("<!-- EXECUTION-ORDER-START -->" in body, f"issue#{issue_number}", "M317-B001-GH-ISSUE-MARKER", "execution-order marker missing", failures)
        checks_passed += require(len(payload.get("labels", [])) > 0, f"issue#{issue_number}", "M317-B001-GH-ISSUE-LABELS", "labels missing", failures)

    for milestone_code, milestone_number in [("M288", 373), ("M293", 378), ("M304", 389), ("M317", 398)]:
        payload = gh_api_payload(f"repos/{REPO}/milestones/{milestone_number}")
        desc = payload.get("description") or ""
        checks_total += 2
        checks_passed += require("<!-- EXECUTION-ORDER-START -->" in desc, f"milestone#{milestone_number}", "M317-B001-GH-MILESTONE-MARKER", "execution-order marker missing", failures)
        checks_passed += require(payload.get("title", "").startswith(milestone_code), f"milestone#{milestone_number}", "M317-B001-GH-MILESTONE-TITLE", f"milestone title drifted: {payload.get('title', '')}", failures)

    issue_list = run_command(["gh", "issue", "list", "--state", "open", "--limit", "500", "--json", "number,labels,body"])
    checks_total += 3
    checks_passed += require(issue_list.returncode == 0, "gh issue list", "M317-B001-GH-OPEN-01", issue_list.stderr or issue_list.stdout or "gh issue list failed", failures)
    open_payload = json.loads(issue_list.stdout) if issue_list.returncode == 0 else []
    unlabeled = [item["number"] for item in open_payload if not item.get("labels")]
    missing_execution = [item["number"] for item in open_payload if "<!-- EXECUTION-ORDER-START -->" not in (item.get("body") or "")]
    checks_passed += require(len(unlabeled) == model["metadata_targets"]["expected_open_issue_unlabeled"], "gh issue list", "M317-B001-GH-OPEN-02", f"unlabeled open issues remain: {unlabeled[:10]}", failures)
    checks_passed += require(len(missing_execution) == model["metadata_targets"]["expected_open_issue_missing_execution_order"], "gh issue list", "M317-B001-GH-OPEN-03", f"open issues missing execution markers: {missing_execution[:10]}", failures)

    return checks_total, checks_passed, {
        "issues": issue_payloads,
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
            SnippetCheck("M317-B001-EXP-01", "# M317 Immediate Backlog Surgery and Supersession Model Contract and Architecture Freeze Expectations (B001)"),
            SnippetCheck("M317-B001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M317-B001-EXP-03", "`narrow_in_place`"),
            SnippetCheck("M317-B001-EXP-04", "`concept_hold`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M317-B001-PKT-01", "# M317-B001 Packet: Immediate backlog surgery and supersession model - Contract and architecture freeze"),
            SnippetCheck("M317-B001-PKT-02", "corrective_tranche_consumes_existing_scope"),
            SnippetCheck("M317-B001-PKT-03", "Next issue: `M317-B002`."),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M317-B001-PKG-01", '"check:objc3c:m317-b001-immediate-backlog-surgery-and-supersession-model-contract-and-architecture-freeze"'),
            SnippetCheck("M317-B001-PKG-02", '"test:tooling:m317-b001-immediate-backlog-surgery-and-supersession-model-contract-and-architecture-freeze"'),
            SnippetCheck("M317-B001-PKG-03", '"check:objc3c:m317-b001-lane-b-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    model = load_json(MODEL_JSON)
    mapping = load_json(MAP_JSON)
    checks_total += 9
    checks_passed += require(model.get("mode") == MODE, display_path(MODEL_JSON), "M317-B001-MODEL-01", "mode drifted", failures)
    checks_passed += require(model.get("contract_id") == CONTRACT_ID, display_path(MODEL_JSON), "M317-B001-MODEL-02", "contract id drifted", failures)
    checks_passed += require(model.get("dependency_contract_id") == mapping.get("contract_id"), display_path(MODEL_JSON), "M317-B001-MODEL-03", "dependency contract mismatch", failures)
    allowed_actions = model.get("allowed_actions", [])
    checks_passed += require(isinstance(allowed_actions, list) and len(allowed_actions) == 6, display_path(MODEL_JSON), "M317-B001-MODEL-04", "allowed action count drifted", failures)
    allowed_action_names = {item["action"] for item in allowed_actions}
    checks_passed += require({"narrow_in_place", "corrective_tranche_consumes_existing_scope", "boundary_preserve", "cleanup_first_block", "concept_hold", "future_dependency_rewrite"} == allowed_action_names, display_path(MODEL_JSON), "M317-B001-MODEL-05", "allowed action set drifted", failures)
    prohibited_patterns = model.get("prohibited_patterns", [])
    checks_passed += require(isinstance(prohibited_patterns, list) and len(prohibited_patterns) == 5, display_path(MODEL_JSON), "M317-B001-MODEL-06", "prohibited pattern count drifted", failures)
    checks_passed += require(any("M277-B002" in item for item in prohibited_patterns), display_path(MODEL_JSON), "M317-B001-MODEL-07", "M277-B002 prohibition missing", failures)
    checks_passed += require(any("M288" in item for item in prohibited_patterns), display_path(MODEL_JSON), "M317-B001-MODEL-08", "M288 prohibition missing", failures)
    checks_passed += require(model.get("metadata_targets", {}).get("expected_open_issue_unlabeled") == 0 and model.get("metadata_targets", {}).get("expected_open_issue_missing_execution_order") == 0, display_path(MODEL_JSON), "M317-B001-MODEL-09", "metadata targets drifted", failures)

    github_summary: dict[str, object] = {"skipped": True}
    github_executed = False
    if not args.skip_github_probes:
        github_executed = True
        gh_total, gh_passed, github_summary = run_github_probes(model, failures)
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
        "allowed_action_names": sorted(allowed_action_names),
        "prohibited_pattern_count": len(prohibited_patterns),
        "github_summary": github_summary,
        "next_issue": "M317-B002",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-B001 immediate backlog surgery model checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
