#!/usr/bin/env python3
"""Checker for M317-A001 backlog overlap and correction inventory."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m317-a001-backlog-overlap-correction-inventory-v1"
CONTRACT_ID = "objc3c-cleanup-backlog-overlap-correction-inventory/m317-a001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-A001" / "backlog_overlap_correction_inventory_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_backlog_overlap_and_correction_inventory_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_a001_backlog_overlap_and_correction_inventory_contract_and_architecture_freeze_packet.md"
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_a001_backlog_overlap_and_correction_inventory_contract_and_architecture_freeze_inventory.json"
GUIDANCE_DOC = ROOT / "tmp" / "planning" / "cleanup_acceleration_program" / "github_issue_alteration_guidance.md"
EXECUTION_ORDER_DOC = ROOT / "tmp" / "planning" / "cleanup_acceleration_program" / "execution_order.md"
METADATA_POLICY_DOC = ROOT / "tmp" / "planning" / "cleanup_acceleration_program" / "open_issue_metadata_policy.md"
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
        failures.append(Finding(display_path(path), "M317-A001-MISSING", f"missing artifact: {display_path(path)}"))
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


def run_github_probes(failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    issue_expectations = {
        7399: [
            "narrow architecture-level retirement inventory",
            "should not absorb the later execution work",
        ],
        7421: [
            "selector pool",
            "`objc_msgSend`-equivalent path",
        ],
        7425: [
            "selector-thunk or equivalent method-dispatch mechanics",
            "reflection publication",
        ],
        7428: [
            "executable message dispatch",
            "runtime probes",
        ],
        7434: [
            "synthesized getter/setter semantics",
            "ownership-aware setters",
        ],
        7438: [
            "real getter and setter LLVM IR bodies",
            "missing synthesized accessor getter binding",
        ],
        7441: [
            "live synthesized getter/setter execution",
            "observable object-state change",
        ],
        7764: [
            "Concept-only; blocked until post-M308 tranche is materially complete",
        ],
    }
    issue_summaries: dict[str, Any] = {}
    for issue_number, snippets in issue_expectations.items():
        payload = gh_issue_payload(issue_number)
        issue_summaries[str(issue_number)] = {
            "title": payload["title"],
            "label_count": len(payload.get("labels", [])),
        }
        body = payload.get("body") or ""
        checks_total += 2 + len(snippets)
        checks_passed += require("<!-- EXECUTION-ORDER-START -->" in body, f"issue#{issue_number}", "M317-A001-GH-ISSUE-MARKER", "execution-order marker missing", failures)
        checks_passed += require(len(payload.get("labels", [])) > 0, f"issue#{issue_number}", "M317-A001-GH-ISSUE-LABELS", "labels missing", failures)
        for idx, snippet in enumerate(snippets, start=1):
            checks_passed += require(snippet in body, f"issue#{issue_number}", f"M317-A001-GH-ISSUE-{issue_number}-{idx:02d}", f"missing body snippet: {snippet}", failures)

    milestone_expectations = {
        362: "Blocked until cleanup-first sequence complete",
        364: "Blocked until cleanup-first sequence complete",
        365: "Blocked until cleanup-first sequence complete",
        373: "Blocked until cleanup-first sequence complete",
        378: "Blocked until cleanup-first sequence complete",
        398: "Cleanup program phase: `1/6`",
    }
    milestone_summaries: dict[str, Any] = {}
    for milestone_number, snippet in milestone_expectations.items():
        payload = gh_api_payload(f"repos/{REPO}/milestones/{milestone_number}")
        desc = payload.get("description") or ""
        milestone_summaries[str(milestone_number)] = {"title": payload.get("title", "")}
        checks_total += 2
        checks_passed += require("<!-- EXECUTION-ORDER-START -->" in desc, f"milestone#{milestone_number}", "M317-A001-GH-MILESTONE-MARKER", "execution-order marker missing from milestone description", failures)
        checks_passed += require(snippet in desc, f"milestone#{milestone_number}", "M317-A001-GH-MILESTONE-SNIPPET", f"missing milestone description snippet: {snippet}", failures)

    issue_list = run_command(["gh", "issue", "list", "--state", "open", "--limit", "500", "--json", "number,labels,body"])
    checks_total += 3
    checks_passed += require(issue_list.returncode == 0, "gh issue list", "M317-A001-GH-OPEN-01", issue_list.stderr or issue_list.stdout or "gh issue list failed", failures)
    open_payload = json.loads(issue_list.stdout) if issue_list.returncode == 0 else []
    unlabeled = [item["number"] for item in open_payload if not item.get("labels")]
    missing_execution = [item["number"] for item in open_payload if "<!-- EXECUTION-ORDER-START -->" not in (item.get("body") or "")]
    checks_passed += require(len(unlabeled) == 0, "gh issue list", "M317-A001-GH-OPEN-02", f"unlabeled open issues remain: {unlabeled[:10]}", failures)
    checks_passed += require(len(missing_execution) == 0, "gh issue list", "M317-A001-GH-OPEN-03", f"open issues missing execution markers: {missing_execution[:10]}", failures)

    return checks_total, checks_passed, {
        "issues": issue_summaries,
        "milestones": milestone_summaries,
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
            SnippetCheck("M317-A001-EXP-01", "# M317 Backlog Overlap and Correction Inventory Contract and Architecture Freeze Expectations (A001)"),
            SnippetCheck("M317-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M317-A001-EXP-03", "`0` unlabeled open issues"),
        ],
        PACKET_DOC: [
            SnippetCheck("M317-A001-PKT-01", "# M317-A001 Packet: Backlog overlap and correction inventory - Contract and architecture freeze"),
            SnippetCheck("M317-A001-PKT-02", "scaffold_retirement_scope_narrowing"),
            SnippetCheck("M317-A001-PKT-03", "future_post_m292_dependency_consumers"),
            SnippetCheck("M317-A001-PKT-04", "Next issue: `M317-A002`."),
        ],
        GUIDANCE_DOC: [
            SnippetCheck("M317-A001-GUI-01", "`#7399`"),
            SnippetCheck("M317-A001-GUI-02", "`#7421`"),
            SnippetCheck("M317-A001-GUI-03", "`#7438`"),
            SnippetCheck("M317-A001-GUI-04", "`#7529-#7538`"),
            SnippetCheck("M317-A001-GUI-05", "### M293-M304 future work interaction"),
        ],
        EXECUTION_ORDER_DOC: [
            SnippetCheck("M317-A001-EXE-01", "`M317 -> M313 -> M314 -> M315 -> M318 -> M316`"),
            SnippetCheck("M317-A001-EXE-02", "1. `M317-A001` blocked by: none"),
        ],
        METADATA_POLICY_DOC: [
            SnippetCheck("M317-A001-META-01", "All open roadmap issues outside `M313-M318` are blocked by the cleanup-first sequence"),
            SnippetCheck("M317-A001-META-02", "Future seeded open issues should not be left unlabeled or without explicit blocker status."),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M317-A001-PKG-01", '"check:objc3c:m317-a001-backlog-overlap-and-correction-inventory-contract-and-architecture-freeze"'),
            SnippetCheck("M317-A001-PKG-02", '"test:tooling:m317-a001-backlog-overlap-and-correction-inventory-contract-and-architecture-freeze"'),
            SnippetCheck("M317-A001-PKG-03", '"check:objc3c:m317-a001-lane-a-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    inventory = load_json(INVENTORY_JSON)
    checks_total += 13
    checks_passed += require(inventory.get("mode") == MODE, display_path(INVENTORY_JSON), "M317-A001-INV-01", "mode drifted", failures)
    checks_passed += require(inventory.get("contract_id") == CONTRACT_ID, display_path(INVENTORY_JSON), "M317-A001-INV-02", "contract id drifted", failures)
    checks_passed += require(inventory.get("global_cleanup_sequence") == ["M317", "M313", "M314", "M315", "M318", "M316"], display_path(INVENTORY_JSON), "M317-A001-INV-03", "cleanup sequence drifted", failures)
    families = inventory.get("inventory_families", [])
    checks_passed += require(isinstance(families, list) and len(families) == 5, display_path(INVENTORY_JSON), "M317-A001-INV-04", "inventory family count drifted", failures)
    family_ids = {item.get("family_id") for item in families if isinstance(item, dict)}
    checks_passed += require("scaffold_retirement_scope_narrowing" in family_ids, display_path(INVENTORY_JSON), "M317-A001-INV-05", "scaffold overlap family missing", failures)
    checks_passed += require("realized_dispatch_corrective_overlap" in family_ids, display_path(INVENTORY_JSON), "M317-A001-INV-06", "realized dispatch overlap family missing", failures)
    checks_passed += require("synthesized_accessor_corrective_overlap" in family_ids, display_path(INVENTORY_JSON), "M317-A001-INV-07", "synthesized accessor overlap family missing", failures)
    checks_passed += require("human_facing_superclean_boundary" in family_ids, display_path(INVENTORY_JSON), "M317-A001-INV-08", "M288 boundary family missing", failures)
    checks_passed += require("future_post_m292_dependency_consumers" in family_ids, display_path(INVENTORY_JSON), "M317-A001-INV-09", "future consumer family missing", failures)
    critical_issues = set(inventory.get("critical_overlap_issue_numbers", []))
    checks_passed += require({7399, 7421, 7425, 7428, 7434, 7438, 7441}.issubset(critical_issues), display_path(INVENTORY_JSON), "M317-A001-INV-10", "critical runtime overlap issue set drifted", failures)
    future_milestones = set(inventory.get("future_consumer_milestones", []))
    checks_passed += require({"M293", "M304"}.issubset(future_milestones), display_path(INVENTORY_JSON), "M317-A001-INV-11", "future consumer milestone set drifted", failures)
    repo_facts = inventory.get("observed_repo_facts", {})
    checks_passed += require(repo_facts.get("npm_script_count") == 7737, display_path(INVENTORY_JSON), "M317-A001-INV-12", "npm script baseline drifted", failures)
    checks_passed += require(repo_facts.get("dead_prototype_compiler_file") == "compiler/objc3c/semantic.py", display_path(INVENTORY_JSON), "M317-A001-INV-13", "dead prototype compiler path drifted", failures)

    github_summary: dict[str, object] = {"skipped": True}
    github_executed = False
    if not args.skip_github_probes:
        github_executed = True
        gh_total, gh_passed, github_summary = run_github_probes(failures)
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
        "inventory_family_ids": sorted(family_ids),
        "critical_overlap_issue_numbers": sorted(critical_issues),
        "future_consumer_milestones": sorted(future_milestones),
        "github_summary": github_summary,
        "next_issue": "M317-A002",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-A001 backlog overlap inventory checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
