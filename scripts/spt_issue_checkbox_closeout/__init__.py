"""SPT checkbox-to-issue closeout helpers."""

from __future__ import annotations

from .cli import IssueOperations, main, parse_args
from .config import (
    CLOSE_CONFIG_SECTION,
    DEFAULT_CLOSE_CONFIG,
    ConfigError,
    load_close_tooling_config,
)
from .github import close_issue, fetch_open_spt_issues, run_cmd, run_gh_json
from .markdown import checkbox_state, load_catalog, source_line_result
from .models import (
    CatalogTask,
    CloseToolingConfig,
    IssueRef,
    LoadedPlan,
    PlannedAction,
    SourceLineResult,
)
from .planning import (
    build_plan_payload,
    build_planned_actions,
    compute_plan_digest,
    compute_file_digest,
    compute_source_line_hash,
    display_path,
    load_plan,
    normalize_source_line_hash,
    write_plan,
)

__all__ = [
    "CLOSE_CONFIG_SECTION",
    "DEFAULT_CLOSE_CONFIG",
    "CatalogTask",
    "CloseToolingConfig",
    "ConfigError",
    "IssueOperations",
    "IssueRef",
    "LoadedPlan",
    "PlannedAction",
    "SourceLineResult",
    "build_plan_payload",
    "build_planned_actions",
    "checkbox_state",
    "close_issue",
    "compute_file_digest",
    "compute_plan_digest",
    "compute_source_line_hash",
    "display_path",
    "fetch_open_spt_issues",
    "load_catalog",
    "load_close_tooling_config",
    "load_plan",
    "main",
    "normalize_source_line_hash",
    "parse_args",
    "run_cmd",
    "run_gh_json",
    "source_line_result",
    "write_plan",
]
