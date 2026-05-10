from __future__ import annotations

from .models import CatalogTask, IssueRef, LoadedPlan, PlannedAction, SourceLineResult
from .paths import display_path
from .planning import build_planned_actions


def build_checked_open_actions(
    filtered: list[CatalogTask],
    *,
    source_state_by_task_id: dict[str, SourceLineResult],
    open_map: dict[str, IssueRef],
    limit: int | None,
) -> list[PlannedAction]:
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
    if limit is not None:
        actions = actions[: max(0, limit)]
    return build_planned_actions(actions)


def validate_loaded_plan_actions(
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


def build_closeout_comment(action: PlannedAction, commit_sha: str | None) -> str:
    comment_lines = [
        "## Automated Closeout",
        f"- Task ID: `{action.task_id}`",
        f"- Source checkbox confirmed checked: `{action.source_path}:{action.source_line}`",
        f"- Source line: `{action.source_checkbox_line}`",
        "- Closeout mode: checklist-to-issue reconciliation",
    ]
    if commit_sha:
        comment_lines.append(f"- Commit evidence: `{commit_sha}`")

    return "\n".join(comment_lines)
