#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
TMP = ROOT / "tmp"
PLANNING = TMP / "planning"
PUBLISH = TMP / "github-publish"
REPORT_DIR = TMP / "reports" / "m317" / "M317-A001"
JSON_OUT = REPORT_DIR / "backlog_overlap_supersession_inventory.json"
MD_OUT = REPORT_DIR / "backlog_overlap_supersession_inventory.md"
REPO = "doublemover/Slopjective-C"


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


def list_open_milestones() -> list[dict[str, Any]]:
    return run_json([
        "gh",
        "api",
        f"repos/{REPO}/milestones?state=open&per_page=100",
        "--paginate",
        "--slurp",
    ])


def list_open_issues() -> list[dict[str, Any]]:
    return run_json([
        "gh",
        "issue",
        "list",
        "--repo",
        REPO,
        "--state",
        "open",
        "--limit",
        "500",
        "--json",
        "number,title,milestone,labels",
    ])


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


def main() -> int:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)

    master = read_json(PLANNING / "draft_backlog_master_snapshot.json")
    cleanup = read_json(PLANNING / "cleanup_acceleration_program" / "count_snapshot.json")
    runtime = read_json(PLANNING / "runtime_envelope_completion_program" / "count_snapshot.json")
    post = read_json(PLANNING / "post_m324_adoption_program" / "count_snapshot.json")

    milestone_pages = list_open_milestones()
    open_milestones = flatten_pages(milestone_pages)
    open_issues = list_open_issues()

    milestone_number_by_code = {}
    for milestone in open_milestones:
        title = milestone.get("title", "")
        if isinstance(title, str) and title.startswith("M"):
            code = title.split(" ", 1)[0]
            milestone_number_by_code[code] = milestone.get("number")

    m317_issues = [
        {
            "number": issue["number"],
            "title": issue["title"],
        }
        for issue in open_issues
        if isinstance(issue.get("title"), str) and issue["title"].startswith("[M317]")
    ]

    inventory = {
        "mode": "m317-a001-backlog-overlap-supersession-publication-scope-inventory-v2",
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "repo": REPO,
        "count_sources": {
            "draft_backlog_master_snapshot": str((PLANNING / "draft_backlog_master_snapshot.json").relative_to(ROOT)).replace("\\", "/"),
            "cleanup_count_snapshot": str((PLANNING / "cleanup_acceleration_program" / "count_snapshot.json").relative_to(ROOT)).replace("\\", "/"),
            "runtime_count_snapshot": str((PLANNING / "runtime_envelope_completion_program" / "count_snapshot.json").relative_to(ROOT)).replace("\\", "/"),
            "post_count_snapshot": str((PLANNING / "post_m324_adoption_program" / "count_snapshot.json").relative_to(ROOT)).replace("\\", "/"),
        },
        "current_counts": {
            "local_total_milestones": master["total_milestones"],
            "local_total_issues": master["total_issues"],
            "cleanup_milestones": cleanup["milestone_count"],
            "cleanup_issues": cleanup["issue_count"],
            "runtime_milestones": runtime["milestone_count"],
            "runtime_issues": runtime["issue_count"],
            "post_m324_milestones": post["milestone_count"],
            "post_m324_issues": post["issue_count"],
            "github_open_milestones": len(open_milestones),
            "github_open_issues": len(open_issues),
            "github_open_m317_issues": len(m317_issues),
            "package_json_scripts": master["repo_facts"]["package_scripts"],
            "check_py_files": master["repo_facts"]["check_py_files"],
            "test_check_py_files": master["repo_facts"]["test_check_py_files"],
            "m2xx_refs_in_native_src": master["repo_facts"]["m2xx_refs_in_native_src"],
            "tracked_ll_files": master["repo_facts"]["tracked_ll_files"],
            "tracked_stub_ll_files": master["repo_facts"]["tracked_stub_ll_files"],
        },
        "publication_scope": {
            "internal_first_milestones": ["M317"],
            "cleanup_overlap_group": ["M313", "M314", "M315"],
            "recommended_sequence": [
                "M317",
                "M313",
                "M314",
                "M315",
                "M316",
                "M318",
                "M319",
                "M320",
                "M321",
                "M322",
                "M323",
                "M324",
                "M325",
                "M326",
                "M327",
                "M330",
                "M329",
                "M328",
                "M331",
                "M332",
            ],
        },
        "live_milestone_numbers": milestone_number_by_code,
        "m317_live_issues": m317_issues,
        "overlap_families": [
            {
                "family_id": "unified_draft_program_generation",
                "owner": "tmp/planning/regenerate_draft_programs.py",
                "status": "active",
                "supersedes": [
                    "tmp/planning/cleanup_acceleration_program/generate_cleanup_acceleration_program.py"
                ],
                "reason": "All three draft programs now share one manifest-driven generator; the cleanup-only generator is redundant and drift-prone.",
            },
            {
                "family_id": "manifest_driven_publication_pipeline",
                "owner": "tmp/github-publish/publish_draft_backlog_programs.py + tmp/github-publish/finalize_draft_backlog_publication.py",
                "status": "active",
                "supersedes": [
                    "tmp/github-publish/final_runtime_completion_program/publish_final_runtime_completion_program.py",
                    "tmp/github-publish/post_m292_refined_program/publish_post_m292_refined_program.py"
                ],
                "reason": "The new backlog set is published from generated manifests and dependency graphs, not from milestone-specific hardcoded publisher scripts.",
            },
            {
                "family_id": "cleanup_tranche_live_reuse",
                "owner": "live GitHub milestones M313-M318",
                "status": "active",
                "affected_milestones": ["M313", "M314", "M315", "M316", "M317", "M318"],
                "reason": "These milestones already existed on GitHub and were reopened/updated in place instead of being recreated with invented numbers.",
            },
            {
                "family_id": "runtime_envelope_publication",
                "owner": "runtime_envelope_completion_program manifests",
                "status": "active",
                "affected_milestones": ["M319", "M320", "M321", "M322", "M323", "M324"],
                "reason": "The runtime-envelope tranche is now structurally separated from cleanup and post-adoption work, with direct blocker metadata carried in the generated manifests.",
            },
            {
                "family_id": "post_m324_adoption_publication",
                "owner": "post_m324_adoption_program manifests",
                "status": "active",
                "affected_milestones": ["M325", "M326", "M327", "M328", "M329", "M330", "M331", "M332"],
                "reason": "Post-closure durability and adoption work now has its own manifest-driven program and no longer relies on the older post-M292 hardcoded publication packet.",
            },
        ],
        "non_goals": [
            "This inventory does not define the supersession policy itself; that belongs to M317-B001.",
            "This inventory does not change issue templates or publication helpers beyond identifying the active owner surfaces.",
            "This inventory does not re-open archived tmp/archive milestone-era artifacts; it classifies them as historical references only.",
        ],
        "next_issue": "M317-B001",
    }

    JSON_OUT.write_text(json.dumps(inventory, indent=2) + "\n", encoding="utf-8")

    lines = [
        "# M317-A001 Backlog Overlap, Supersession, And Publication-Scope Inventory",
        "",
        f"- generated_at: `{inventory['generated_at']}`",
        f"- github_open_milestones: `{inventory['current_counts']['github_open_milestones']}`",
        f"- github_open_issues: `{inventory['current_counts']['github_open_issues']}`",
        f"- github_open_m317_issues: `{inventory['current_counts']['github_open_m317_issues']}`",
        f"- local_total_milestones: `{inventory['current_counts']['local_total_milestones']}`",
        f"- local_total_issues: `{inventory['current_counts']['local_total_issues']}`",
        "",
        "## Current measured facts",
        f"- `package.json` scripts: `{inventory['current_counts']['package_json_scripts']}`",
        f"- `check_*.py` files: `{inventory['current_counts']['check_py_files']}`",
        f"- `test_check_*.py` files: `{inventory['current_counts']['test_check_py_files']}`",
        f"- `m2xx-*` refs in `native/objc3c/src`: `{inventory['current_counts']['m2xx_refs_in_native_src']}`",
        f"- tracked `.ll` files: `{inventory['current_counts']['tracked_ll_files']}`",
        f"- tracked `*stub*.ll` files: `{inventory['current_counts']['tracked_stub_ll_files']}`",
        "",
        "## Active owner surfaces",
    ]
    for family in inventory["overlap_families"]:
        lines.extend([
            f"- `{family['family_id']}`",
            f"  owner: `{family['owner']}`",
            f"  status: `{family['status']}`",
            f"  reason: {family['reason']}",
        ])
        if "supersedes" in family:
            lines.append(f"  supersedes: {', '.join(f'`{item}`' for item in family['supersedes'])}")
        if "affected_milestones" in family:
            lines.append(f"  milestones: {', '.join(f'`{item}`' for item in family['affected_milestones'])}")
    lines.extend([
        "",
        "## Live M317 issues",
    ])
    for issue in m317_issues:
        lines.append(f"- `#{issue['number']}` {issue['title']}")
    lines.extend([
        "",
        "## Non-goals",
    ])
    lines.extend(f"- {item}" for item in inventory["non_goals"])
    lines.extend([
        "",
        f"Next issue: `{inventory['next_issue']}`",
    ])
    MD_OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
