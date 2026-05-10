from __future__ import annotations

import argparse
import re
import sys
from collections.abc import Callable
from dataclasses import dataclass
from pathlib import Path

from .config import load_close_tooling_config
from .github import close_issue, fetch_open_spt_issues
from .markdown import load_catalog, source_line_result
from .models import CatalogTask, IssueRef, LoadedPlan, PlannedAction, SourceLineResult
from .paths import ROOT
from .planning import (
    build_plan_payload,
    build_planned_actions,
    compute_file_digest,
    display_path,
    load_plan,
    normalize_source_line_hash,
    write_plan,
)


@dataclass(frozen=True)
class IssueOperations:
    fetch_open_spt_issues: Callable[[re.Pattern[str]], dict[str, IssueRef]]
    close_issue: Callable[[int, str], None]


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


def _resolve_optional_root_path(path: Path | None) -> Path | None:
    if path is None:
        return None
    if path.is_absolute():
        return path
    return ROOT / path


def _filter_catalog_tasks(
    catalog_tasks: list[CatalogTask],
    *,
    lane: str | None,
    task_id_prefix: str,
) -> list[CatalogTask]:
    if lane:
        filtered = [task for task in catalog_tasks if f"[Lane {lane}]" in task.title]
    else:
        filtered = catalog_tasks

    return [task for task in filtered if task.task_id.startswith(task_id_prefix)]


def _collect_source_state(
    filtered: list[CatalogTask],
) -> tuple[list[tuple[CatalogTask, str]], dict[str, SourceLineResult], dict[str, CatalogTask]]:
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

    return stale_entries, source_state_by_task_id, task_by_task_id


def _print_stale_source_diagnostics(stale_entries: list[tuple[CatalogTask, str]]) -> None:
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


def _validate_loaded_plan_actions(
    loaded_plan: LoadedPlan,
    task_by_task_id: dict[str, CatalogTask],
    source_state_by_task_id: dict[str, SourceLineResult],
) -> list[str]:
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

    return invalid_plan_actions


def _print_invalid_plan_actions(invalid_plan_actions: list[str]) -> None:
    for entry in invalid_plan_actions:
        print(entry, file=sys.stderr)
    print(
        f"error: detected {len(invalid_plan_actions)} invalid plan action(s); aborting apply",
        file=sys.stderr,
    )


def _default_operations() -> IssueOperations:
    return IssueOperations(
        fetch_open_spt_issues=fetch_open_spt_issues,
        close_issue=close_issue,
    )


def main(argv: list[str] | None = None, operations: IssueOperations | None = None) -> int:
    args = parse_args(argv)
    issue_operations = operations or _default_operations()

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
    plan_out_path = _resolve_optional_root_path(args.plan_out)
    plan_in_path = _resolve_optional_root_path(args.plan_in)

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

    filtered = _filter_catalog_tasks(
        catalog_tasks,
        lane=effective_lane,
        task_id_prefix=effective_task_id_prefix,
    )

    stale_entries, source_state_by_task_id, task_by_task_id = _collect_source_state(filtered)

    if args.fail_on_stale_source and stale_entries:
        _print_stale_source_diagnostics(stale_entries)
        return 2

    planned_actions: list[PlannedAction]
    if loaded_plan is not None:
        invalid_plan_actions = _validate_loaded_plan_actions(
            loaded_plan,
            task_by_task_id,
            source_state_by_task_id,
        )

        if invalid_plan_actions:
            _print_invalid_plan_actions(invalid_plan_actions)
            return 2

        planned_actions = loaded_plan.actions
    else:
        open_map = issue_operations.fetch_open_spt_issues(config.task_id_pattern)
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
        issue_operations.close_issue(action.issue_number, comment)
        closed += 1
        print(f"closed #{action.issue_number} ({action.task_id})")

    print(f"closed_total={closed}")
    return 0
