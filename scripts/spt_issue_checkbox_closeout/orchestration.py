from __future__ import annotations

import sys

from .arguments import parse_args
from .closeout import (
    build_checked_open_actions,
    build_closeout_comment,
    validate_loaded_plan_actions,
)
from .checkbox_state import collect_source_state
from .config import load_close_tooling_config
from .loading import filter_catalog_tasks, resolve_catalog_path
from .loading import resolve_optional_root_path
from .markdown import load_catalog
from .models import LoadedPlan, PlannedAction
from .operations import IssueOperations, default_operations
from .planning import (
    build_plan_payload,
    compute_file_digest,
    load_plan,
    normalize_source_line_hash,
    write_plan,
)
from .reporting import (
    print_action_matches,
    print_action_summary,
    print_closed_issue,
    print_closed_total,
    print_error,
    print_invalid_plan_actions,
    print_plan_freeze_hint,
    print_plan_written,
    print_stale_source_diagnostics,
)


def _load_apply_plan(args, plan_in_path) -> LoadedPlan | None:
    if not args.apply:
        return None
    if plan_in_path is None:
        print_error("--plan-in path resolution failed")
        return None
    try:
        return load_plan(plan_in_path)
    except RuntimeError as exc:
        print_error(str(exc))
        return None


def _validate_apply_plan_options(args, loaded_plan: LoadedPlan) -> bool:
    if args.lane is not None and args.lane != loaded_plan.lane_filter:
        print_error(
            f"lane filter mismatch between CLI ({args.lane}) and plan ({loaded_plan.lane_filter})"
        )
        return False
    if args.task_id_prefix is not None and args.task_id_prefix != loaded_plan.task_id_prefix:
        print_error("task-id-prefix mismatch between CLI and plan")
        return False
    if args.commit_sha and args.commit_sha != (loaded_plan.commit_sha or ""):
        print_error("commit-sha mismatch between CLI and plan")
        return False
    return True


def _print_catalog_digest_mismatch(loaded_plan: LoadedPlan, live_catalog_digest: str) -> None:
    print_error("plan/catalog digest mismatch; plan is stale against current catalog")
    print(
        f"plan_catalog_digest={loaded_plan.catalog_digest} live_catalog_digest={live_catalog_digest}",
        file=sys.stderr,
    )


def _build_planned_actions(
    *,
    loaded_plan: LoadedPlan | None,
    filtered,
    source_state_by_task_id,
    task_by_task_id,
    issue_operations: IssueOperations,
    task_id_pattern,
    limit: int | None,
) -> list[PlannedAction] | None:
    if loaded_plan is not None:
        invalid_plan_actions = validate_loaded_plan_actions(
            loaded_plan,
            task_by_task_id,
            source_state_by_task_id,
        )

        if invalid_plan_actions:
            print_invalid_plan_actions(invalid_plan_actions)
            return None

        return loaded_plan.actions

    open_map = issue_operations.fetch_open_spt_issues(task_id_pattern)
    return build_checked_open_actions(
        filtered,
        source_state_by_task_id=source_state_by_task_id,
        open_map=open_map,
        limit=limit,
    )


def main(argv: list[str] | None = None, operations: IssueOperations | None = None) -> int:
    args = parse_args(argv)
    issue_operations = operations or default_operations()

    if args.apply and args.plan_in is None:
        print_error("--apply requires --plan-in <path>")
        return 2
    if args.apply and args.plan_out is not None:
        print_error("--plan-out cannot be used with --apply")
        return 2

    config = load_close_tooling_config(args.config)

    catalog = resolve_catalog_path(args.catalog, config.catalog_default)
    plan_out_path = resolve_optional_root_path(args.plan_out)
    plan_in_path = resolve_optional_root_path(args.plan_in)

    loaded_plan = _load_apply_plan(args, plan_in_path)
    if args.apply:
        if loaded_plan is None:
            return 2
        if not _validate_apply_plan_options(args, loaded_plan):
            return 2

    effective_lane = loaded_plan.lane_filter if loaded_plan is not None else args.lane
    effective_task_id_prefix = (
        loaded_plan.task_id_prefix
        if loaded_plan is not None
        else (
            args.task_id_prefix
            if args.task_id_prefix is not None
            else config.task_id_prefix_default
        )
    )
    effective_commit_sha = (
        loaded_plan.commit_sha
        if loaded_plan is not None
        else (args.commit_sha or None)
    )

    catalog_tasks = load_catalog(catalog)
    live_catalog_digest = compute_file_digest(catalog)
    if loaded_plan is not None and normalize_source_line_hash(
        loaded_plan.catalog_digest
    ) != normalize_source_line_hash(live_catalog_digest):
        _print_catalog_digest_mismatch(loaded_plan, live_catalog_digest)
        return 2

    filtered = filter_catalog_tasks(
        catalog_tasks,
        lane=effective_lane,
        task_id_prefix=effective_task_id_prefix,
    )

    source_state = collect_source_state(filtered)

    if args.fail_on_stale_source and source_state.stale_entries:
        print_stale_source_diagnostics(source_state.stale_entries)
        return 2

    planned_actions = _build_planned_actions(
        loaded_plan=loaded_plan,
        filtered=filtered,
        source_state_by_task_id=source_state.source_state_by_task_id,
        task_by_task_id=source_state.task_by_task_id,
        issue_operations=issue_operations,
        task_id_pattern=config.task_id_pattern,
        limit=args.limit,
    )
    if planned_actions is None:
        return 2

    print_action_summary(
        catalog_count=len(catalog_tasks),
        filtered_count=len(filtered),
        action_count=len(planned_actions),
        stale_source_count=len(source_state.stale_entries),
    )
    print_action_matches(planned_actions)

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
        print_plan_written(plan_out_path, len(planned_actions), payload["plan_digest"])
        return 0

    if not args.apply:
        print_plan_freeze_hint()
        return 0

    closed = 0
    for action in planned_actions:
        comment = build_closeout_comment(action, effective_commit_sha)
        issue_operations.close_issue(action.issue_number, comment)
        closed += 1
        print_closed_issue(action.issue_number, action.task_id)

    print_closed_total(closed)
    return 0
