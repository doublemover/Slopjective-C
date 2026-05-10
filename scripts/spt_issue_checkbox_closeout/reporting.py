from __future__ import annotations

import sys
from pathlib import Path

from .models import CatalogTask, PlannedAction
from .paths import display_path


def print_error(message: str) -> None:
    print(f"error: {message}", file=sys.stderr)


def print_stale_source_diagnostics(stale_entries: list[tuple[CatalogTask, str]]) -> None:
    for task, reason in stale_entries:
        rel = display_path(task.path)
        key_info = f" task_key={task.task_key}" if task.task_key else ""
        hash_info = f" source_line_hash={task.source_line_hash}" if task.source_line_hash else ""
        print(
            f"stale_source {task.task_id}{key_info} source={rel}:{task.line}{hash_info} reason={reason}",
            file=sys.stderr,
        )
    print_error(
        f"detected {len(stale_entries)} stale source entries; "
        "rerun catalog generation or update source references."
    )


def print_invalid_plan_actions(invalid_plan_actions: list[str]) -> None:
    for entry in invalid_plan_actions:
        print(entry, file=sys.stderr)
    print_error(f"detected {len(invalid_plan_actions)} invalid plan action(s); aborting apply")


def print_action_summary(
    *,
    catalog_count: int,
    filtered_count: int,
    action_count: int,
    stale_source_count: int,
) -> None:
    print(
        f"catalog_tasks={catalog_count} filtered={filtered_count} "
        f"checked_open_matches={action_count} stale_source={stale_source_count}"
    )


def print_action_matches(planned_actions: list[PlannedAction]) -> None:
    for action in planned_actions:
        print(
            f"match {action.task_id} -> #{action.issue_number} ({action.issue_url}) "
            f"source={action.source_path}:{action.source_line}"
        )


def print_plan_written(path: Path, action_count: int, plan_digest: str) -> None:
    print(f"plan_written path={display_path(path)} actions={action_count} digest={plan_digest}")
    print("dry-run only; pass --apply --plan-in <path> to close issues")


def print_plan_freeze_hint() -> None:
    print("dry-run only; pass --plan-out <path> to freeze close actions")


def print_closed_issue(issue_number: int, task_id: str) -> None:
    print(f"closed #{issue_number} ({task_id})")


def print_closed_total(closed: int) -> None:
    print(f"closed_total={closed}")
