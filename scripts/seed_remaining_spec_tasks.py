#!/usr/bin/env python3
"""Review unchecked spec tasks and seed organized GitHub issues.

This script does three things in one deterministic pass:
1. Extract every unchecked checkbox task from spec/*.md files.
2. Generate a comprehensive review catalog with improved task definitions.
3. Optionally create GitHub issues for each task with lane/milestone metadata.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import time
from datetime import date
from pathlib import Path
from typing import Any

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from remaining_task_extraction.seed_config import (
    ConfigError,
    load_seed_tooling_config,
    resolve_generated_on,
)
from remaining_task_extraction.seed_review import (
    RawTask,
    ReviewedTask,
    format_source_reference,
    review_tasks,
)

ROOT = SCRIPT_ROOT.parents[0]
SPEC_ROOT = ROOT / "spec"

CHECKBOX_PATTERN = re.compile(r"^- \[ \]\s+(.*)$")
PLANNING_ISSUE_PATTERN = re.compile(r"issue_(\d+)_")
LEGACY_CONFORMANCE_PROFILE_SOURCE = (
    "docs/reference/legacy_spec_anchor_index.md#conformance-profile-checklist"
)
LEGACY_PLANNING_SOURCE = "docs/reference/legacy_spec_anchor_index.md"


class GhError(RuntimeError):
    pass


def run_cmd(args: list[str], input_text: str | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        args,
        cwd=ROOT,
        input=input_text,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_gh_json(args: list[str], input_payload: dict[str, Any] | None = None) -> Any:
    input_text = None
    full_args = ["gh", *args]
    if input_payload is not None:
        full_args.extend(["--input", "-"])
        input_text = json.dumps(input_payload)

    proc = run_cmd(full_args, input_text=input_text)
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise GhError(f"{' '.join(full_args)} failed: {detail}")

    stdout = proc.stdout.strip()
    if not stdout:
        return None

    try:
        return json.loads(stdout)
    except json.JSONDecodeError as exc:
        raise GhError(f"{' '.join(full_args)} returned invalid JSON: {exc}") from exc


def extract_raw_tasks() -> list[RawTask]:
    tasks: list[RawTask] = []
    for path in sorted(SPEC_ROOT.rglob("*.md")):
        rel = path.relative_to(ROOT).as_posix()
        if rel == "spec/CONFORMANCE_PROFILE_CHECKLIST.md":
            source_rel = LEGACY_CONFORMANCE_PROFILE_SOURCE
        elif PLANNING_ISSUE_PATTERN.search(rel):
            source_rel = LEGACY_PLANNING_SOURCE
        else:
            source_rel = rel
        lines = path.read_text(encoding="utf-8").splitlines()
        for idx, line in enumerate(lines, start=1):
            match = CHECKBOX_PATTERN.match(line)
            if not match:
                continue
            tasks.append(
                RawTask(
                    path=source_rel,
                    line=idx,
                    text=match.group(1).strip(),
                    origin_path=rel,
                )
            )
    return tasks


def render_catalog_markdown(
    tasks: list[ReviewedTask],
    lane_name: dict[str, str],
    generated_on: date,
) -> str:
    lane_counts: dict[str, int] = {"A": 0, "B": 0, "C": 0, "D": 0}
    bucket_counts: dict[str, int] = {}
    quality_counts: dict[str, int] = {}

    for task in tasks:
        lane_counts[task.lane] = lane_counts.get(task.lane, 0) + 1
        bucket_counts[task.bucket] = bucket_counts.get(task.bucket, 0) + 1
        quality_counts[task.quality] = quality_counts.get(task.quality, 0) + 1

    lines: list[str] = [
        "# Remaining Task Review Catalog (510-Task Sweep)",
        "",
        f"_Generated on {generated_on.isoformat()} by scripts/seed_remaining_spec_tasks.py._",
        "",
        "## Coverage Summary",
        "",
        f"- Total reviewed tasks: **{len(tasks)}**",
        f"- Bucket counts: conformance-checklist **{bucket_counts.get('conformance-checklist', 0)}**, release-evidence **{bucket_counts.get('release-evidence', 0)}**, planning-checklist **{bucket_counts.get('planning-checklist', 0)}**",
        f"- Lane counts: A **{lane_counts.get('A', 0)}**, B **{lane_counts.get('B', 0)}**, C **{lane_counts.get('C', 0)}**, D **{lane_counts.get('D', 0)}**",
        f"- Definition quality counts: strong **{quality_counts.get('strong', 0)}**, medium **{quality_counts.get('medium', 0)}**, weak **{quality_counts.get('weak', 0)}**",
        "",
        "## Parallel Worklane Guidance",
        "",
        "- Lane A: normative/spec closure tasks with cross-part semantic contracts.",
        "- Lane B: implementation/tooling and conformance automation tasks.",
        "- Lane C: governance, extension process, and ecosystem policy tasks.",
        "- Lane D: release readiness, checkpoints, handoff, and operational control tasks.",
        "",
        "## Task Reviews",
        "",
    ]

    for task in tasks:
        lines.append(f"### {task.task_id} - {task.title}")
        lines.append("")
        lines.append(f"- Source: `{format_source_reference(task)}`")
        lines.append(f"- Bucket: `{task.bucket}`")
        lines.append(f"- Lane: `Lane {task.lane} - {lane_name[task.lane]}`")
        lines.append(f"- Milestone: `{task.milestone_title}`")
        lines.append(f"- Shard: `{task.shard}`")
        lines.append(f"- Original checkbox: `{task.original}`")
        lines.append(f"- Review quality: `{task.quality}`")
        if task.quality_gaps:
            lines.append("- Gaps identified:")
            for gap in task.quality_gaps:
                lines.append(f"  - {gap}")
        else:
            lines.append("- Gaps identified: none; wording already has strong structure.")
        lines.append(f"- Improved objective: {task.objective}")
        lines.append("- Improved deliverables:")
        for item in task.deliverables:
            lines.append(f"  - {item}")
        lines.append("- Improved acceptance criteria:")
        for item in task.acceptance_criteria:
            lines.append(f"  - {item}")
        lines.append("- Dependencies:")
        for item in task.dependencies:
            lines.append(f"  - {item}")
        lines.append("- Validation commands:")
        for cmd in task.validation_commands:
            lines.append(f"  - `{cmd}`")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def serialize_catalog_json(
    tasks: list[ReviewedTask],
    lane_name: dict[str, str],
    generated_on: date,
) -> str:
    payload = {
        "generated_on": generated_on.isoformat(),
        "task_count": len(tasks),
        "tasks": [
            {
                "task_id": task.task_id,
                "task_key": task.task_key,
                "source_line_hash": task.source_line_hash,
                "title": task.title,
                "path": task.path,
                "line": task.line,
                "bucket": task.bucket,
                "lane": task.lane,
                "lane_name": lane_name[task.lane],
                "milestone_title": task.milestone_title,
                "priority_label": task.priority_label,
                "area_label": task.area_label,
                "type_label": task.type_label,
                "labels": task.labels,
                "quality": task.quality,
                "quality_gaps": task.quality_gaps,
                "original": task.original,
                "cleaned": task.cleaned,
                "objective": task.objective,
                "deliverables": task.deliverables,
                "acceptance_criteria": task.acceptance_criteria,
                "dependencies": task.dependencies,
                "validation_commands": task.validation_commands,
                "shard": task.shard,
                "execution_status": task.execution_status,
            }
            for task in tasks
        ],
    }
    return json.dumps(payload, indent=2) + "\n"


def build_issue_body(task: ReviewedTask, lane_name: dict[str, str]) -> str:
    quality_line = {
        "strong": "Strong baseline wording; improvements focus on execution rigor and evidence shape.",
        "medium": "Partially defined; improvements add missing acceptance and validation precision.",
        "weak": "Underspecified; improvements convert this into an outcome-driven implementation task.",
    }[task.quality]

    gap_lines = task.quality_gaps or [
        "No structural gaps detected in source wording; issue still adds deterministic execution metadata."
    ]

    parallel_with = ", ".join(lane for lane in ["A", "B", "C", "D"] if lane != task.lane)

    body = [
        f"## Source Traceability",
        f"- Task ID: `{task.task_id}`",
        f"- Source: `{format_source_reference(task)}`",
        f"- Source bucket: `{task.bucket}`",
        f"- Original checkbox: `{task.original}`",
        "",
        "## Definition Review",
        f"- Assessment: **{task.quality.upper()}**",
        f"- Summary: {quality_line}",
        "- What I would do differently:",
    ]

    for gap in gap_lines:
        body.append(f"  - {gap}")

    body.extend(
        [
            "",
            "## Improved Task Definition",
            f"### Objective\n{task.objective}",
            "",
            "### Deliverables",
        ]
    )
    for item in task.deliverables:
        body.append(f"- {item}")

    body.append("")
    body.append("### Acceptance Criteria")
    for idx, item in enumerate(task.acceptance_criteria, start=1):
        body.append(f"{idx}. {item}")

    body.append("")
    body.append("### Dependencies")
    for item in task.dependencies:
        body.append(f"- {item}")

    body.append("")
    body.append("### Validation Commands")
    for cmd in task.validation_commands:
        body.append(f"- `{cmd}`")

    body.extend(
        [
            "",
            "## Parallel Execution Plan",
            f"- Primary lane: **Lane {task.lane} - {lane_name[task.lane]}**",
            f"- Parallelizable with lanes: **{parallel_with}**",
            f"- Suggested shard key: `{task.shard}` (batch same-shard tasks to reduce context switching)",
            "",
            "## Closeout Evidence Required",
            "- Commit SHA(s) implementing the task",
            "- Paths to produced artifacts/tests/evidence",
            "- Validation command outputs",
            "- Checklist row update reference",
        ]
    )

    return "\n".join(body).strip() + "\n"


def ensure_labels(repo: str, label_defs: dict[str, tuple[str, str]]) -> None:
    existing = run_gh_json(["label", "list", "--limit", "500", "--json", "name"])
    existing_names = {
        item["name"] for item in existing if isinstance(item, dict) and isinstance(item.get("name"), str)
    }

    for name, (color, description) in label_defs.items():
        if name in existing_names:
            continue
        run_gh_json(
            [
                "api",
                f"repos/{repo}/labels",
                "-X",
                "POST",
                "-f",
                f"name={name}",
                "-f",
                f"color={color}",
                "-f",
                f"description={description}",
            ]
        )


def fetch_milestone_map(repo: str) -> dict[str, int]:
    payload = run_gh_json(
        ["api", f"repos/{repo}/milestones?state=all&per_page=100"]
    )
    mapping: dict[str, int] = {}
    if isinstance(payload, list):
        for item in payload:
            if not isinstance(item, dict):
                continue
            title = item.get("title")
            number = item.get("number")
            if isinstance(title, str) and isinstance(number, int):
                mapping[title] = number
    return mapping


def ensure_lane_milestones(
    repo: str,
    lane_milestone_titles: dict[str, str],
    lane_milestone_due_on: dict[str, str],
    lane_name: dict[str, str],
) -> dict[str, int]:
    mapping = fetch_milestone_map(repo)
    for lane, title in lane_milestone_titles.items():
        if title in mapping:
            continue
        description = (
            f"Parallel lane {lane} ({lane_name[lane]}) task batch generated from the 510-task unchecked spec sweep."
        )
        due_on = lane_milestone_due_on[lane]
        payload = run_gh_json(
            ["api", f"repos/{repo}/milestones", "-X", "POST"],
            input_payload={
                "title": title,
                "description": description,
                "due_on": due_on,
            },
        )
        if isinstance(payload, dict) and isinstance(payload.get("number"), int):
            mapping[title] = int(payload["number"])

    return mapping


def fetch_existing_seeded_task_ids() -> set[str]:
    payload = run_gh_json(
        ["issue", "list", "--state", "all", "--limit", "2000", "--json", "title"]
    )
    ids: set[str] = set()
    if not isinstance(payload, list):
        return ids

    pattern = re.compile(r"^\[(SPT-\d{4})\]")
    for item in payload:
        if not isinstance(item, dict):
            continue
        title = item.get("title")
        if not isinstance(title, str):
            continue
        match = pattern.match(title)
        if match:
            ids.add(match.group(1))
    return ids


def create_issue(repo: str, payload: dict[str, Any]) -> dict[str, Any]:
    retries = 6
    wait_seconds = 1.0
    for attempt in range(1, retries + 1):
        proc = run_cmd(
            ["gh", "api", f"repos/{repo}/issues", "-X", "POST", "--input", "-"],
            input_text=json.dumps(payload),
        )
        if proc.returncode == 0:
            try:
                result = json.loads(proc.stdout)
            except json.JSONDecodeError as exc:
                raise GhError(f"Issue creation returned non-JSON output: {exc}") from exc
            if not isinstance(result, dict):
                raise GhError("Issue creation returned unexpected JSON shape")
            return result

        stderr = proc.stderr.strip() or proc.stdout.strip()
        too_fast = "secondary rate limit" in stderr.lower() or "abuse detection" in stderr.lower()
        transient = "502" in stderr or "503" in stderr or "504" in stderr

        if attempt < retries and (too_fast or transient):
            sleep_for = max(wait_seconds, 60.0 if too_fast else wait_seconds)
            time.sleep(sleep_for)
            wait_seconds *= 2
            continue

        raise GhError(f"Issue creation failed: {stderr or f'exit {proc.returncode}'}")

    raise GhError("Issue creation failed after retries")


def seed_issues(
    repo: str,
    tasks: list[ReviewedTask],
    milestone_map: dict[str, int],
    lane_name: dict[str, str],
    sleep_seconds: float,
    limit: int | None,
) -> tuple[int, int]:
    existing_ids = fetch_existing_seeded_task_ids()

    created = 0
    skipped = 0
    processed = 0

    for task in tasks:
        if limit is not None and processed >= limit:
            break
        processed += 1

        if task.task_id in existing_ids:
            skipped += 1
            continue

        milestone_number = milestone_map.get(task.milestone_title)
        if milestone_number is None:
            raise GhError(
                f"Missing milestone mapping for '{task.milestone_title}' while seeding {task.task_id}"
            )

        payload = {
            "title": task.title,
            "body": build_issue_body(task, lane_name),
            "milestone": milestone_number,
            "labels": task.labels,
        }

        result = create_issue(repo, payload)
        number = result.get("number")
        url = result.get("html_url")
        if isinstance(number, int) and isinstance(url, str):
            print(f"created #{number} {task.task_id} {url}")
        else:
            print(f"created {task.task_id}")

        created += 1
        time.sleep(sleep_seconds)

    return created, skipped


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="seed_remaining_spec_tasks.py",
        description=(
            "Review all unchecked spec tasks, generate improved task definitions, "
            "and optionally seed GitHub issues with milestones/lanes."
        ),
    )
    parser.add_argument(
        "--config",
        type=Path,
        default=None,
        help=(
            "Optional path to tooling config JSON. When omitted, built-in defaults are used; "
            "built-in defaults match pre-HB-06 behavior."
        ),
    )
    parser.add_argument(
        "--repo",
        default=None,
        help=(
            "GitHub repository in owner/name format. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--catalog-md",
        type=Path,
        default=None,
        help=(
            "Path to write markdown review catalog. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=None,
        help=(
            "Path to write JSON review catalog. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--create-issues",
        action="store_true",
        help="Create GitHub issues for reviewed tasks.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Optional limit for issue creation (for staged runs).",
    )
    parser.add_argument(
        "--sleep-seconds",
        type=float,
        default=None,
        help=(
            "Delay between issue creations to reduce rate-limit risk. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--generated-on",
        default=None,
        help=(
            "Date to embed in catalog outputs in YYYY-MM-DD format. "
            "When omitted, SOURCE_DATE_EPOCH is honored if present; otherwise today's date is used."
        ),
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    config = load_seed_tooling_config(args.config, root=ROOT)

    repo = args.repo if args.repo is not None else config.repo_default
    catalog_md = args.catalog_md if args.catalog_md is not None else config.catalog_md_default
    catalog_json = args.catalog_json if args.catalog_json is not None else config.catalog_json_default
    sleep_seconds = args.sleep_seconds if args.sleep_seconds is not None else config.sleep_seconds_default
    generated_on = resolve_generated_on(args.generated_on)

    raw_tasks = extract_raw_tasks()
    reviewed = review_tasks(raw_tasks, config)

    if len(reviewed) != config.expected_task_count:
        print(
            f"warning: expected {config.expected_task_count} tasks, found {len(reviewed)}",
            file=sys.stderr,
        )

    catalog_md.parent.mkdir(parents=True, exist_ok=True)
    catalog_json.parent.mkdir(parents=True, exist_ok=True)

    catalog_md.write_text(
        render_catalog_markdown(reviewed, config.lane_name, generated_on),
        encoding="utf-8",
    )
    catalog_json.write_text(
        serialize_catalog_json(reviewed, config.lane_name, generated_on),
        encoding="utf-8",
    )

    print(
        f"catalog_written tasks={len(reviewed)} md={catalog_md.as_posix()} json={catalog_json.as_posix()}"
    )

    if not args.create_issues:
        return 0

    ensure_labels(repo, config.label_defs)
    milestone_map = ensure_lane_milestones(
        repo,
        config.lane_milestone_titles,
        config.lane_milestone_due_on,
        config.lane_name,
    )
    milestone_map = fetch_milestone_map(repo)

    created, skipped = seed_issues(
        repo=repo,
        tasks=reviewed,
        milestone_map=milestone_map,
        lane_name=config.lane_name,
        sleep_seconds=sleep_seconds,
        limit=args.limit,
    )

    print(
        f"issue_seed_complete created={created} skipped_existing={skipped} total_reviewed={len(reviewed)}"
    )
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (ConfigError, GhError, RuntimeError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2)
