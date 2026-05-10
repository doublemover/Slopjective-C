"""Public CLI facade for remaining-task backlog extraction."""

from __future__ import annotations

import sys
from typing import Sequence

from objc3c_tooling.json_io import render_json
from remaining_task_extraction.catalog import (
    build_groups,
    build_payload,
    capacity_row_to_dict,
    capacity_status_severity,
    capacity_status_sort_key,
    classify_capacity_status,
    classify_threshold_status,
    compute_capacity_rows,
    compute_dispatch_intake_status,
    compute_overlap_conflicts,
    count_by,
    dedupe_sort,
    dispatch_state_for_status,
    filter_rows,
    group_value_sort_key,
    intake_recommendation_for_status,
    lane_sort_key,
    load_catalog_rows,
    normalize_catalog_path,
    normalize_inline_text,
    normalize_lane,
    normalize_lane_filter,
    normalize_status,
    normalize_status_filter,
    overlap_conflict_row_to_dict,
    parse_positive_line,
    ratio,
    row_sort_key,
    row_to_dict,
    status_sort_key,
    text_sort_key,
)
from remaining_task_extraction.cli_arguments import build_parser
from remaining_task_extraction.cli_output import write_stdout
from remaining_task_extraction.cli_paths import DEFAULT_INPUT, ROOT, resolve_input_path
from remaining_task_extraction.model import (
    CAPACITY_STATUS_ORDER_INDEX,
    CAPACITY_STATUS_SORT_ORDER,
    DEFAULT_MAX_OVERLAP_CONFLICTS,
    DEFAULT_STATUSES,
    GLOBAL_WIP_CAP,
    HARD_CAP_BREACH_LOAD_RATIO,
    LANE_ORDER_INDEX,
    LANE_SORT_ORDER,
    LANE_WIP_CAPS,
    OVERLAP_ACTIVE_STATUS,
    STATUS_ORDER_INDEX,
    STATUS_SORT_ORDER,
    WARNING_LOAD_RATIO,
    LaneCapacityRow,
    OverlapConflictRow,
    TaskRow,
)
from remaining_task_extraction.rendering import (
    format_counts,
    format_filter_values,
    format_ratio,
    markdown_safe_cell,
    render_markdown,
)

__all__ = [
    "CAPACITY_STATUS_ORDER_INDEX",
    "CAPACITY_STATUS_SORT_ORDER",
    "DEFAULT_INPUT",
    "DEFAULT_MAX_OVERLAP_CONFLICTS",
    "DEFAULT_STATUSES",
    "GLOBAL_WIP_CAP",
    "HARD_CAP_BREACH_LOAD_RATIO",
    "LANE_ORDER_INDEX",
    "LANE_SORT_ORDER",
    "LANE_WIP_CAPS",
    "OVERLAP_ACTIVE_STATUS",
    "ROOT",
    "STATUS_ORDER_INDEX",
    "STATUS_SORT_ORDER",
    "WARNING_LOAD_RATIO",
    "LaneCapacityRow",
    "OverlapConflictRow",
    "TaskRow",
    "build_groups",
    "build_parser",
    "build_payload",
    "capacity_row_to_dict",
    "capacity_status_severity",
    "capacity_status_sort_key",
    "classify_capacity_status",
    "classify_threshold_status",
    "compute_capacity_rows",
    "compute_dispatch_intake_status",
    "compute_overlap_conflicts",
    "count_by",
    "dedupe_sort",
    "dispatch_state_for_status",
    "filter_rows",
    "format_counts",
    "format_filter_values",
    "format_ratio",
    "group_value_sort_key",
    "intake_recommendation_for_status",
    "lane_sort_key",
    "load_catalog_rows",
    "main",
    "markdown_safe_cell",
    "normalize_catalog_path",
    "normalize_inline_text",
    "normalize_lane",
    "normalize_lane_filter",
    "normalize_status",
    "normalize_status_filter",
    "overlap_conflict_row_to_dict",
    "parse_positive_line",
    "ratio",
    "render_json",
    "render_markdown",
    "resolve_input_path",
    "row_sort_key",
    "row_to_dict",
    "status_sort_key",
    "text_sort_key",
    "write_stdout",
]


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        if args.max_overlap_conflicts < 0:
            raise ValueError("--max-overlap-conflicts must be >= 0")
        status_filters = normalize_status_filter(args.status_filters)
        lane_filters = normalize_lane_filter(args.lane_filters)
        input_path = resolve_input_path(args.input)
        rows = load_catalog_rows(
            input_path,
            allow_missing_status=args.allow_missing_status,
        )
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    filtered = filter_rows(rows, statuses=status_filters, lanes=lane_filters)
    groups = build_groups(filtered, group_by=args.group_by)
    payload = build_payload(
        input_path=input_path,
        all_rows=rows,
        statuses=status_filters,
        lanes=lane_filters,
        group_by=args.group_by,
        groups=groups,
        max_overlap_conflicts=args.max_overlap_conflicts,
    )

    if args.format == "markdown":
        write_stdout(render_markdown(payload))
    else:
        write_stdout(render_json(payload))

    if args.max_dispatch_intake_status is not None:
        summary = payload["summary"]
        assert isinstance(summary, dict)
        dispatch_intake = summary["dispatch_intake"]
        assert isinstance(dispatch_intake, dict)
        status = dispatch_intake["status"]
        assert isinstance(status, str)
        status_severity = capacity_status_severity(status)
        limit_severity = capacity_status_severity(args.max_dispatch_intake_status)
        if status_severity > limit_severity:
            print(
                (
                    "error: dispatch-intake status exceeds threshold; "
                    f"status={status}, max_allowed={args.max_dispatch_intake_status}"
                ),
                file=sys.stderr,
            )
            return 1
    return 0
