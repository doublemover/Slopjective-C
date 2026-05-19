from __future__ import annotations

from .arguments import build_parser, parse_args
from .closeout import (
    build_checked_open_actions,
    build_closeout_comment,
    validate_loaded_plan_actions,
)
from .checkbox_state import CatalogSourceState, collect_source_state
from .config import load_close_tooling_config
from .github import close_issue, fetch_open_spt_issues
from .loading import (
    filter_catalog_tasks,
    resolve_catalog_path,
    resolve_optional_root_path,
)
from .markdown import load_catalog, source_line_result
from .models import CatalogTask, IssueRef, LoadedPlan, PlannedAction, SourceLineResult
from .operations import IssueOperations, default_operations
from .orchestration import main
from .paths import ROOT, display_path
from .planning import (
    build_plan_payload,
    build_planned_actions,
    compute_file_digest,
    load_plan,
    normalize_source_line_hash,
    write_plan,
)
from .reporting import print_invalid_plan_actions, print_stale_source_diagnostics

_collect_source_state = collect_source_state
_default_operations = default_operations
_filter_catalog_tasks = filter_catalog_tasks
_print_invalid_plan_actions = print_invalid_plan_actions
_print_stale_source_diagnostics = print_stale_source_diagnostics
_resolve_catalog_path = resolve_catalog_path
_resolve_optional_root_path = resolve_optional_root_path
_validate_loaded_plan_actions = validate_loaded_plan_actions

__all__ = [
    "IssueOperations",
    "CatalogSourceState",
    "CatalogTask",
    "IssueRef",
    "LoadedPlan",
    "PlannedAction",
    "ROOT",
    "SourceLineResult",
    "build_checked_open_actions",
    "build_closeout_comment",
    "build_parser",
    "build_plan_payload",
    "build_planned_actions",
    "close_issue",
    "collect_source_state",
    "compute_file_digest",
    "default_operations",
    "display_path",
    "fetch_open_spt_issues",
    "filter_catalog_tasks",
    "load_catalog",
    "load_close_tooling_config",
    "load_plan",
    "main",
    "normalize_source_line_hash",
    "parse_args",
    "print_invalid_plan_actions",
    "print_stale_source_diagnostics",
    "resolve_catalog_path",
    "resolve_optional_root_path",
    "source_line_result",
    "validate_loaded_plan_actions",
    "write_plan",
]
