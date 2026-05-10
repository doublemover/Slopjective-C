#!/usr/bin/env python3
"""Generate deterministic compiler milestone dispatch plans from GH snapshots."""

from __future__ import annotations

import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from compiler_dispatch_plan import COMPILER_DISPATCH_ARTIFACT_OWNER
from compiler_dispatch_plan import COMPILER_DISPATCH_CONTRACT_ID
from compiler_dispatch_plan import COMPILER_DISPATCH_FIXTURE_CONTRACT_ID
from compiler_dispatch_plan import COMPILER_DISPATCH_OWNER
from compiler_dispatch_plan import COMPILER_DISPATCH_RESULT_OWNER
from compiler_dispatch_plan import COMPILER_DISPATCH_SNAPSHOT_OWNER
from compiler_dispatch_plan import COMPILER_DISPATCH_STATUS_OWNER
from compiler_dispatch_plan import DEFAULT_ISSUES_JSON
from compiler_dispatch_plan import LANE_ORDER
from compiler_dispatch_plan import OWNER_CONTRACT_FIELDS
from compiler_dispatch_plan import ROOT
from compiler_dispatch_plan import TASK_ID_RE
from compiler_dispatch_plan import IssueRow
from compiler_dispatch_plan import TaskRef
from compiler_dispatch_plan import build_parser
from compiler_dispatch_plan import build_payload
from compiler_dispatch_plan import dispatch_owner_contract
from compiler_dispatch_plan import display_path
from compiler_dispatch_plan import flatten_json_pages
from compiler_dispatch_plan import main
from compiler_dispatch_plan import normalize_path
from compiler_dispatch_plan import normalize_space
from compiler_dispatch_plan import parse_issue_rows
from compiler_dispatch_plan import parse_lane_labels
from compiler_dispatch_plan import parse_task_ref
from compiler_dispatch_plan import pick_target_milestone
from compiler_dispatch_plan import render_json
from compiler_dispatch_plan import render_markdown
from compiler_dispatch_plan import validate_fixture_owner_contract

__all__ = [
    "COMPILER_DISPATCH_ARTIFACT_OWNER",
    "COMPILER_DISPATCH_CONTRACT_ID",
    "COMPILER_DISPATCH_FIXTURE_CONTRACT_ID",
    "COMPILER_DISPATCH_OWNER",
    "COMPILER_DISPATCH_RESULT_OWNER",
    "COMPILER_DISPATCH_SNAPSHOT_OWNER",
    "COMPILER_DISPATCH_STATUS_OWNER",
    "DEFAULT_ISSUES_JSON",
    "LANE_ORDER",
    "OWNER_CONTRACT_FIELDS",
    "ROOT",
    "TASK_ID_RE",
    "IssueRow",
    "TaskRef",
    "build_parser",
    "build_payload",
    "dispatch_owner_contract",
    "display_path",
    "flatten_json_pages",
    "main",
    "normalize_path",
    "normalize_space",
    "parse_issue_rows",
    "parse_lane_labels",
    "parse_task_ref",
    "pick_target_milestone",
    "render_json",
    "render_markdown",
    "validate_fixture_owner_contract",
]


if __name__ == "__main__":
    raise SystemExit(main())
