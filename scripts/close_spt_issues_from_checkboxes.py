#!/usr/bin/env python3
"""Close SPT issues when their source checkbox rows are checked.

This facade preserves the public script path and helper names while the
implementation lives in `scripts/spt_issue_checkbox_closeout/`.
"""

from __future__ import annotations

import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from spt_issue_checkbox_closeout.cli import IssueOperations, main as _main, parse_args
from spt_issue_checkbox_closeout.config import load_close_tooling_config
from spt_issue_checkbox_closeout.github import (
    close_issue as _close_issue,
    fetch_open_spt_issues as _fetch_open_spt_issues,
    run_cmd as _run_cmd,
    run_gh_json as _run_gh_json,
)
from spt_issue_checkbox_closeout.markdown import checkbox_state, load_catalog, source_line_result
from spt_issue_checkbox_closeout.models import (
    CatalogTask,
    IssueRef,
    LoadedPlan,
    PlannedAction,
    SourceLineResult,
)
from spt_issue_checkbox_closeout.planning import (
    build_plan_payload,
    build_planned_actions,
    compute_file_digest,
    display_path,
    load_plan,
    normalize_source_line_hash,
    write_plan,
)

ROOT = SCRIPT_ROOT.parents[0]


def run_cmd(args: list[str]):
    return _run_cmd(args)


def run_gh_json(args: list[str]):
    return _run_gh_json(args)


def fetch_open_spt_issues(task_id_pattern):
    return _fetch_open_spt_issues(task_id_pattern, gh_json_runner=run_gh_json)


def close_issue(number: int, comment: str) -> None:
    _close_issue(number, comment, command_runner=run_cmd)


def main(argv: list[str] | None = None) -> int:
    operations = IssueOperations(
        fetch_open_spt_issues=fetch_open_spt_issues,
        close_issue=close_issue,
    )
    return _main(argv, operations=operations)


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2)
