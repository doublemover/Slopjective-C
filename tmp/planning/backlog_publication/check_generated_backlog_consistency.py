#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
PLANNING = ROOT / "tmp" / "planning"
PUBLISH = ROOT / "tmp" / "github-publish"
REPORT_DIR = ROOT / "tmp" / "reports" / "m317" / "M317-D001"
JSON_OUT = REPORT_DIR / "backlog_consistency_audit.json"
MD_OUT = REPORT_DIR / "backlog_consistency_audit.md"
REPO = "doublemover/Slopjective-C"
PROGRAM_DIRS = [
    ("cleanup_acceleration_program", "cleanup_acceleration_program"),
    ("runtime_envelope_completion_program", "runtime_envelope_completion_program"),
    ("post_m324_adoption_program", "post_m324_adoption_program"),
]
CODE_RE = re.compile(r"^\[(M\d+)\]\[Lane-([A-E])\]\[([A-Z]\d{3})\] ")


def run_json(command: list[str]) -> Any:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"command failed: {' '.join(command)}")
    return json.loads(completed.stdout)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object in {path}")
    return payload


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def flatten_pages(payload: list[Any]) -> list[dict[str, Any]]:
    result: list[dict[str, Any]] = []
    for page in payload:
        if isinstance(page, list):
            for item in page:
                if isinstance(item, dict):
                    result.append(item)
        elif isinstance(page, dict):
            result.append(page)
    return result


def parse_issue_code(title: str) -> str | None:
    match = CODE_RE.match(title)
    if not match:
        return None
    return f"{match.group(1)}-{match.group(3)}"


def main() -> int:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    mismatches: list[str] = []
    program_results: list[dict[str, Any]] = []

    master_snapshot = read_json(PLANNING / "draft_backlog_master_snapshot.json")
    expected_total_milestones = 0
    expected_total_issues = 0
    manifest_titles: dict[str, str] = {}
    manifest_milestones: dict[str, str] = {}

    for planning_dir_name, publish_dir_name in PROGRAM_DIRS:
        planning_dir = PLANNING / planning_dir_name
        publish_dir = PUBLISH / publish_dir_name
        count_snapshot = read_json(planning_dir / "count_snapshot.json")
        manifest = read_json(publish_dir / "program_manifest.json")
        dependency_graph = read_json(publish_dir / "dependency_edges.json")

        if manifest.get("count_snapshot") != str((planning_dir / "count_snapshot.json").relative_to(ROOT)).replace("\\", "/"):
            mismatches.append(f"{planning_dir_name}: program_manifest count_snapshot path mismatch")
        if manifest.get("program_code") != planning_dir_name:
            mismatches.append(f"{planning_dir_name}: program_manifest program_code mismatch")
        if dependency_graph.get("program_code") != planning_dir_name:
            mismatches.append(f"{planning_dir_name}: dependency_graph program_code mismatch")
        if count_snapshot.get("program_code") != planning_dir_name:
            mismatches.append(f"{planning_dir_name}: count_snapshot program_code mismatch")

        milestones = manifest.get("milestones", [])
        expected_total_milestones += len(milestones)
        issue_total = 0

        milestone_edge_pairs = {(edge["from"], edge["to"]) for edge in dependency_graph.get("milestone_edges", [])}
        issue_edge_pairs = {(edge["from"], edge["to"]) for edge in dependency_graph.get("issue_edges", [])}

        for milestone in milestones:
            milestone_code = milestone["code"]
            manifest_milestones[milestone_code] = milestone["title"]
            blocked_by_milestones = milestone.get("blocked_by_milestones", [])
            for blocker in blocked_by_milestones:
                if (blocker, milestone_code) not in milestone_edge_pairs:
                    mismatches.append(f"{planning_dir_name}:{milestone_code} missing milestone edge {blocker}->{milestone_code}")
            issues = milestone.get("issues", [])
            issue_total += len(issues)
            if milestone.get("issue_count") != len(issues):
                mismatches.append(f"{planning_dir_name}:{milestone_code} issue_count mismatch")
            if count_snapshot["issue_counts_by_milestone"].get(milestone_code) != len(issues):
                mismatches.append(f"{planning_dir_name}:{milestone_code} count_snapshot issue count mismatch")

            for issue in issues:
                issue_code = issue["code"]
                manifest_titles[issue_code] = issue["title"]
                issue_md_path = planning_dir / "issues" / f"{issue_code}.md"
                issue_json_path = publish_dir / "issue_bodies" / f"{issue_code}.json"
                if not issue_md_path.exists():
                    mismatches.append(f"{issue_code}: missing planning issue markdown")
                    continue
                if not issue_json_path.exists():
                    mismatches.append(f"{issue_code}: missing publish issue json")
                    continue
                issue_md = read_text(issue_md_path)
                issue_json = read_json(issue_json_path)
                if issue.get("body") != issue_md:
                    mismatches.append(f"{issue_code}: manifest body != markdown body")
                if issue_json.get("body") != issue_md:
                    mismatches.append(f"{issue_code}: issue json body != markdown body")
                if issue_json.get("blocked_by_issue_codes") != issue.get("blocked_by_issue_codes"):
                    mismatches.append(f"{issue_code}: issue json blockers != manifest blockers")
                if issue_json.get("direct_unblock_issue_codes") != issue.get("direct_unblock_issue_codes"):
                    mismatches.append(f"{issue_code}: issue json direct-unblocks != manifest direct-unblocks")

                incoming = sorted(edge_from for edge_from, edge_to in issue_edge_pairs if edge_to == issue_code)
                outgoing = sorted(edge_to for edge_from, edge_to in issue_edge_pairs if edge_from == issue_code)
                if sorted(issue.get("blocked_by_issue_codes", [])) != incoming:
                    mismatches.append(f"{issue_code}: blocked_by_issue_codes do not match dependency graph incoming edges")
                if sorted(issue.get("direct_unblock_issue_codes", [])) != outgoing:
                    mismatches.append(f"{issue_code}: direct_unblock_issue_codes do not match dependency graph outgoing edges")

        if count_snapshot.get("milestone_count") != len(milestones):
            mismatches.append(f"{planning_dir_name}: milestone_count mismatch")
        if count_snapshot.get("issue_count") != issue_total:
            mismatches.append(f"{planning_dir_name}: issue_count mismatch")

        expected_total_issues += issue_total
        program_results.append(
            {
                "program_code": planning_dir_name,
                "milestone_count": len(milestones),
                "issue_count": issue_total,
                "manifest_path": str((publish_dir / "program_manifest.json").relative_to(ROOT)).replace("\\", "/"),
                "dependency_graph_path": str((publish_dir / "dependency_edges.json").relative_to(ROOT)).replace("\\", "/"),
                "count_snapshot_path": str((planning_dir / "count_snapshot.json").relative_to(ROOT)).replace("\\", "/"),
            }
        )

    if master_snapshot.get("total_milestones") != expected_total_milestones:
        mismatches.append("master snapshot total_milestones mismatch")
    if master_snapshot.get("total_issues") != expected_total_issues:
        mismatches.append("master snapshot total_issues mismatch")

    open_milestones = flatten_pages(
        run_json([
            "gh",
            "api",
            f"repos/{REPO}/milestones?state=open&per_page=100",
            "--paginate",
            "--slurp",
        ])
    )
    all_issues = run_json([
        "gh",
        "issue",
        "list",
        "--repo",
        REPO,
        "--state",
        "all",
        "--limit",
        "1000",
        "--json",
        "number,title,milestone",
    ])

    live_milestone_map = {
        milestone["title"].split(" ", 1)[0]: milestone["title"]
        for milestone in open_milestones
        if isinstance(milestone.get("title"), str) and milestone["title"].startswith("M")
    }
    live_issue_codes = {}
    for issue in all_issues:
        title = issue.get("title", "")
        if not isinstance(title, str):
            continue
        issue_code = parse_issue_code(title)
        if issue_code:
            live_issue_codes[issue_code] = title

    if len(open_milestones) != expected_total_milestones:
        mismatches.append(f"live GitHub open milestone count mismatch: {len(open_milestones)} != {expected_total_milestones}")
    live_backlog_issue_count = sum(1 for issue_code in manifest_titles if issue_code in live_issue_codes)
    if live_backlog_issue_count != expected_total_issues:
        mismatches.append(f"live GitHub backlog-coded issue count mismatch: {live_backlog_issue_count} != {expected_total_issues}")

    for milestone_code, milestone_title in manifest_milestones.items():
        if live_milestone_map.get(milestone_code) != f"{milestone_code} {milestone_title}":
            mismatches.append(f"live GitHub milestone title mismatch for {milestone_code}")
    for issue_code, issue_title in manifest_titles.items():
        if live_issue_codes.get(issue_code) != issue_title:
            mismatches.append(f"live GitHub issue title mismatch for {issue_code}")

    report = {
        "issue": "M317-D001",
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "repo": REPO,
        "programs": program_results,
        "expected_total_milestones": expected_total_milestones,
        "expected_total_issues": expected_total_issues,
        "live_github_open_milestones": len(open_milestones),
        "live_github_backlog_coded_issues": live_backlog_issue_count,
        "mismatch_count": len(mismatches),
        "mismatches": mismatches,
        "passed": not mismatches,
    }

    JSON_OUT.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# M317-D001 Backlog Consistency Audit",
        "",
        f"- generated_at: `{report['generated_at']}`",
        f"- expected_total_milestones: `{expected_total_milestones}`",
        f"- expected_total_issues: `{expected_total_issues}`",
        f"- live_github_open_milestones: `{len(open_milestones)}`",
        f"- live_github_backlog_coded_issues: `{live_backlog_issue_count}`",
        f"- mismatch_count: `{len(mismatches)}`",
        f"- passed: `{report['passed']}`",
        "",
        "## Programs",
    ]
    for result in program_results:
        lines.append(
            f"- `{result['program_code']}`: `{result['milestone_count']}` milestones, `{result['issue_count']}` issues"
        )
    lines.extend(["", "## Mismatches"])
    if mismatches:
        lines.extend(f"- {item}" for item in mismatches)
    else:
        lines.append("- none")
    MD_OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")

    return 0 if not mismatches else 1


if __name__ == "__main__":
    raise SystemExit(main())
