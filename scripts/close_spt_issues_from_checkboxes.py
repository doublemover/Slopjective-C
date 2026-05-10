#!/usr/bin/env python3
"""Close SPT issues when their source checkbox rows are checked.

This helper reconciles `tmp/reports/remaining_task_review_catalog.json` against
current markdown checkbox state in the workspace and can optionally close matching
open SPT issues on GitHub.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from spt_issue_closure.config import load_close_tooling_config
from spt_issue_closure.models import (
    CatalogTask,
    IssueRef,
    LoadedPlan,
    PlannedAction,
    SourceLineResult,
)
from spt_issue_closure.plan import (
    build_plan_payload,
    build_planned_actions,
    compute_file_digest,
    display_path,
    load_plan,
    normalize_source_line_hash,
    source_line_result,
    write_plan,
)

ROOT = SCRIPT_ROOT.parents[0]


def run_cmd(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        args,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_gh_json(args: list[str]) -> Any:
    proc = run_cmd(["gh", *args])
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise RuntimeError(f"gh {' '.join(args)} failed: {detail}")

    try:
        return json.loads(proc.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"gh {' '.join(args)} returned invalid JSON: {exc}") from exc


def load_catalog(path: Path) -> list[CatalogTask]:
    if not path.exists():
        raise RuntimeError(f"catalog file not found: {path}")

    payload = json.loads(path.read_text(encoding="utf-8"))
    raw_tasks = payload.get("tasks")
    if not isinstance(raw_tasks, list):
        raise RuntimeError("catalog JSON missing 'tasks' array")

    tasks: list[CatalogTask] = []
    for raw in raw_tasks:
        if not isinstance(raw, dict):
            continue
        task_id = raw.get("task_id")
        raw_path = raw.get("path")
        raw_line = raw.get("line")
        title = raw.get("title")
        task_key = raw.get("task_key")
        source_line_hash = raw.get("source_line_hash")
        if not isinstance(task_id, str):
            continue
        if not isinstance(raw_path, str):
            continue
        if not isinstance(raw_line, int):
            continue
        if not isinstance(title, str):
            title = task_id
        if task_key is not None:
            if not isinstance(task_key, str) or not task_key:
                raise RuntimeError(
                    f"catalog task '{task_id}' has invalid task_key; expected non-empty string"
                )
        if source_line_hash is not None:
            if not isinstance(source_line_hash, str) or not source_line_hash:
                raise RuntimeError(
                    f"catalog task '{task_id}' has invalid source_line_hash; expected non-empty string"
                )
        tasks.append(
            CatalogTask(
                task_id=task_id,
                path=ROOT / Path(raw_path),
                line=raw_line,
                title=title,
                task_key=task_key,
                source_line_hash=source_line_hash,
            )
        )

    return tasks


def fetch_open_spt_issues(task_id_pattern: re.Pattern[str]) -> dict[str, IssueRef]:
    payload = run_gh_json(
        ["issue", "list", "--state", "open", "--limit", "2000", "--json", "number,title,url"]
    )
    if not isinstance(payload, list):
        raise RuntimeError("unexpected gh issue list payload shape")

    mapping: dict[str, IssueRef] = {}
    for item in payload:
        if not isinstance(item, dict):
            continue
        title = item.get("title")
        number = item.get("number")
        url = item.get("url")
        if not isinstance(title, str) or not isinstance(number, int) or not isinstance(url, str):
            continue
        match = task_id_pattern.match(title)
        if not match:
            continue
        task_id = match.group(1)
        mapping[task_id] = IssueRef(number=number, title=title, url=url)

    return mapping


def checkbox_state(path: Path, line: int) -> tuple[bool, str]:
    result = source_line_result(path, line, expected_hash=None)
    return result.checked, result.display_line


def close_issue(number: int, comment: str) -> None:
    proc = run_cmd(["gh", "issue", "close", str(number), "--comment", comment])
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise RuntimeError(f"failed to close issue #{number}: {detail}")


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="close_spt_issues_from_checkboxes.py",
        description=(
            "Detect checked checklist rows from the 510-task catalog and optionally "
            "close matching open SPT issues."
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
        "--catalog",
        type=Path,
        default=None,
        help=(
            "Path to remaining_task_review_catalog.json. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--task-id-prefix",
        default=None,
        help=(
            "Only consider task IDs with this prefix. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--lane",
        choices=["A", "B", "C", "D"],
        default=None,
        help="Optional lane filter based on issue title tag '[Lane X]'.",
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="Actually close matched open issues. Default is dry-run.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Optional maximum number of issues to close/apply.",
    )
    parser.add_argument(
        "--commit-sha",
        default="",
        help="Optional commit SHA to include in closeout comment.",
    )
    parser.add_argument(
        "--plan-out",
        type=Path,
        default=None,
        help="Write immutable action-plan JSON and exit without closing issues.",
    )
    parser.add_argument(
        "--plan-in",
        type=Path,
        default=None,
        help="Read immutable action-plan JSON; required when --apply is set.",
    )
    parser.add_argument(
        "--fail-on-stale-source",
        action="store_true",
        help=(
            "Exit with code 2 if catalog source references are stale (missing file, line out of "
            "range, or source_line_hash mismatch)."
        ),
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)

    if args.apply and args.plan_in is None:
        print("error: --apply requires --plan-in <path>", file=sys.stderr)
        return 2
    if args.apply and args.plan_out is not None:
        print("error: --plan-out cannot be used with --apply", file=sys.stderr)
        return 2

    config = load_close_tooling_config(args.config)

    catalog = (
        config.catalog_default
        if args.catalog is None
        else (args.catalog if args.catalog.is_absolute() else ROOT / args.catalog)
    )
    plan_out_path = (
        None
        if args.plan_out is None
        else (args.plan_out if args.plan_out.is_absolute() else ROOT / args.plan_out)
    )
    plan_in_path = (
        None
        if args.plan_in is None
        else (args.plan_in if args.plan_in.is_absolute() else ROOT / args.plan_in)
    )

    loaded_plan: LoadedPlan | None = None
    effective_lane = args.lane
    effective_task_id_prefix = (
        args.task_id_prefix if args.task_id_prefix is not None else config.task_id_prefix_default
    )
    effective_commit_sha = args.commit_sha or None

    if args.apply:
        if plan_in_path is None:
            print("error: --plan-in path resolution failed", file=sys.stderr)
            return 2
        try:
            loaded_plan = load_plan(plan_in_path)
        except RuntimeError as exc:
            print(f"error: {exc}", file=sys.stderr)
            return 2
        if args.lane is not None and args.lane != loaded_plan.lane_filter:
            print(
                f"error: lane filter mismatch between CLI ({args.lane}) and plan ({loaded_plan.lane_filter})",
                file=sys.stderr,
            )
            return 2
        if args.task_id_prefix is not None and args.task_id_prefix != loaded_plan.task_id_prefix:
            print(
                "error: task-id-prefix mismatch between CLI and plan",
                file=sys.stderr,
            )
            return 2
        if args.commit_sha and args.commit_sha != (loaded_plan.commit_sha or ""):
            print(
                "error: commit-sha mismatch between CLI and plan",
                file=sys.stderr,
            )
            return 2
        effective_lane = loaded_plan.lane_filter
        effective_task_id_prefix = loaded_plan.task_id_prefix
        effective_commit_sha = loaded_plan.commit_sha

    catalog_tasks = load_catalog(catalog)
    live_catalog_digest = compute_file_digest(catalog)
    if loaded_plan is not None:
        if normalize_source_line_hash(loaded_plan.catalog_digest) != normalize_source_line_hash(
            live_catalog_digest
        ):
            print(
                "error: plan/catalog digest mismatch; plan is stale against current catalog",
                file=sys.stderr,
            )
            print(
                f"plan_catalog_digest={loaded_plan.catalog_digest} live_catalog_digest={live_catalog_digest}",
                file=sys.stderr,
            )
            return 2

    if effective_lane:
        filtered = [
            task
            for task in catalog_tasks
            if f"[Lane {effective_lane}]" in task.title
        ]
    else:
        filtered = catalog_tasks

    filtered = [
        task for task in filtered if task.task_id.startswith(effective_task_id_prefix)
    ]

    stale_entries: list[tuple[CatalogTask, str]] = []
    source_state_by_task_id: dict[str, SourceLineResult] = {}
    task_by_task_id: dict[str, CatalogTask] = {}
    for task in filtered:
        source_state = source_line_result(task.path, task.line, task.source_line_hash)
        source_state_by_task_id[task.task_id] = source_state
        task_by_task_id[task.task_id] = task
        if source_state.stale_reason is not None:
            stale_entries.append((task, source_state.stale_reason))
            continue

    if args.fail_on_stale_source and stale_entries:
        for task, reason in stale_entries:
            rel = display_path(task.path)
            key_info = f" task_key={task.task_key}" if task.task_key else ""
            hash_info = f" source_line_hash={task.source_line_hash}" if task.source_line_hash else ""
            print(
                f"stale_source {task.task_id}{key_info} source={rel}:{task.line}{hash_info} reason={reason}",
                file=sys.stderr,
            )
        print(
            f"error: detected {len(stale_entries)} stale source entries; "
            "rerun catalog generation or update source references.",
            file=sys.stderr,
        )
        return 2

    planned_actions: list[PlannedAction]
    if loaded_plan is not None:
        invalid_plan_actions: list[str] = []
        for action in loaded_plan.actions:
            task = task_by_task_id.get(action.task_id)
            state = source_state_by_task_id.get(action.task_id)
            if task is None or state is None:
                invalid_plan_actions.append(
                    f"plan_action {action.task_id} not found in filtered catalog scope"
                )
                continue
            if state.stale_reason is not None:
                invalid_plan_actions.append(
                    f"plan_action {action.task_id} stale_source={state.stale_reason}"
                )
                continue
            if not state.checked:
                invalid_plan_actions.append(
                    f"plan_action {action.task_id} source checkbox is no longer checked"
                )
                continue
            if action.source_path != display_path(task.path) or action.source_line != task.line:
                invalid_plan_actions.append(
                    f"plan_action {action.task_id} source does not match current catalog entry"
                )
                continue

        if invalid_plan_actions:
            for entry in invalid_plan_actions:
                print(entry, file=sys.stderr)
            print(
                f"error: detected {len(invalid_plan_actions)} invalid plan action(s); aborting apply",
                file=sys.stderr,
            )
            return 2

        planned_actions = loaded_plan.actions
    else:
        open_map = fetch_open_spt_issues(config.task_id_pattern)
        checked_candidates: list[tuple[CatalogTask, IssueRef, str]] = []
        for task in filtered:
            state = source_state_by_task_id[task.task_id]
            if state.stale_reason is not None:
                continue
            issue = open_map.get(task.task_id)
            if issue is None:
                continue
            if not state.checked:
                continue
            checked_candidates.append((task, issue, state.display_line))

        actions = checked_candidates
        if args.limit is not None:
            actions = actions[: max(0, args.limit)]
        planned_actions = build_planned_actions(actions)

    print(
        f"catalog_tasks={len(catalog_tasks)} filtered={len(filtered)} "
        f"checked_open_matches={len(planned_actions)} stale_source={len(stale_entries)}"
    )

    for action in planned_actions:
        print(
            f"match {action.task_id} -> #{action.issue_number} ({action.issue_url}) "
            f"source={action.source_path}:{action.source_line}"
        )

    if plan_out_path is not None:
        payload = build_plan_payload(
            catalog_path=catalog,
            catalog_digest=live_catalog_digest,
            lane_filter=effective_lane,
            task_id_prefix=effective_task_id_prefix,
            commit_sha=effective_commit_sha,
            actions=planned_actions,
        )
        write_plan(plan_out_path, payload)
        print(
            f"plan_written path={display_path(plan_out_path)} actions={len(planned_actions)} "
            f"digest={payload['plan_digest']}"
        )
        print("dry-run only; pass --apply --plan-in <path> to close issues")
        return 0

    if not args.apply:
        print("dry-run only; pass --plan-out <path> to freeze close actions")
        return 0

    closed = 0
    for action in planned_actions:
        comment_lines = [
            "## Automated Closeout",
            f"- Task ID: `{action.task_id}`",
            f"- Source checkbox confirmed checked: `{action.source_path}:{action.source_line}`",
            f"- Source line: `{action.source_checkbox_line}`",
            "- Closeout mode: checklist-to-issue reconciliation",
        ]
        if effective_commit_sha:
            comment_lines.append(f"- Commit evidence: `{effective_commit_sha}`")

        comment = "\n".join(comment_lines)
        close_issue(action.issue_number, comment)
        closed += 1
        print(f"closed #{action.issue_number} ({action.task_id})")

    print(f"closed_total={closed}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2)
