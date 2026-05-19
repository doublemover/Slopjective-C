#!/usr/bin/env python3
"""Run deterministic activation preflight orchestration and persist evidence artifacts."""

from __future__ import annotations

import sys
from typing import Sequence

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from objc3c_tooling.paths import display_path, resolve_repo_path
from scripts.activation_preflight.command_runner import (
    DEFAULT_COMMAND_TIMEOUT_SECONDS,
    run_command,
)
from scripts.activation_preflight.command_specs import (
    activation_check_specs,
    open_blockers_refresh_spec,
    snapshot_refresh_spec,
    spec_lint_spec,
)
from scripts.activation_preflight.contracts import CommandResult, CommandSpec
from scripts.activation_preflight.payload_validation import (
    EXIT_GATE_CLOSED,
    EXIT_GATE_OPEN,
    EXIT_RUNNER_ERROR,
    check_markdown_gate_consistency,
    parse_activation_payload,
)
from scripts.activation_preflight.reports import (
    ACTIVATION_JSON_FILENAME,
    ACTIVATION_MD_FILENAME,
    OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
    REPORT_MD_FILENAME,
    SNAPSHOT_CAPTURE_LOG_FILENAME,
    SPEC_LINT_LOG_FILENAME,
    SUMMARY_JSON_FILENAME,
    build_summary_payload,
    render_command_log,
    render_markdown_report,
    render_spec_lint_log,
)
from scripts.activation_preflight.runner_cli import build_parser
from scripts.activation_preflight.runner_io import write_text
from scripts.activation_preflight.runner_orchestration import run_preflight
from scripts.activation_preflight.runner_paths import ACTIVATION_CHECK_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import CAPTURE_SNAPSHOTS_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import DEFAULT_ACTIONABLE_STATUSES
from scripts.activation_preflight.runner_paths import DEFAULT_CATALOG_JSON
from scripts.activation_preflight.runner_paths import DEFAULT_OPEN_BLOCKERS_ROOT
from scripts.activation_preflight.runner_paths import DEFAULT_OUTPUT_DIR
from scripts.activation_preflight.runner_paths import EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import OPEN_BLOCKERS_REFRESH_RELATIVE_PATH
from scripts.activation_preflight.runner_paths import SPEC_LINT_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import default_open_blockers_output_path
from scripts.activation_preflight.runner_state import determine_final_exit
from scripts.activation_preflight.runner_state import normalize_actionable_statuses
from scripts.activation_preflight.runner_state import parse_non_negative_int


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    return run_preflight(args, command_runner=run_command)


__all__ = [
    "ACTIVATION_CHECK_SCRIPT_PATH",
    "ACTIVATION_JSON_FILENAME",
    "ACTIVATION_MD_FILENAME",
    "CAPTURE_SNAPSHOTS_SCRIPT_PATH",
    "CommandResult",
    "CommandSpec",
    "DEFAULT_ACTIONABLE_STATUSES",
    "DEFAULT_CATALOG_JSON",
    "DEFAULT_COMMAND_TIMEOUT_SECONDS",
    "DEFAULT_OPEN_BLOCKERS_ROOT",
    "DEFAULT_OUTPUT_DIR",
    "EXIT_GATE_CLOSED",
    "EXIT_GATE_OPEN",
    "EXIT_RUNNER_ERROR",
    "EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH",
    "OPEN_BLOCKERS_REFRESH_LOG_FILENAME",
    "OPEN_BLOCKERS_REFRESH_RELATIVE_PATH",
    "REPORT_MD_FILENAME",
    "ROOT",
    "SCRIPT_ROOT",
    "SNAPSHOT_CAPTURE_LOG_FILENAME",
    "SPEC_LINT_LOG_FILENAME",
    "SPEC_LINT_SCRIPT_PATH",
    "SUMMARY_JSON_FILENAME",
    "activation_check_specs",
    "build_parser",
    "build_summary_payload",
    "check_markdown_gate_consistency",
    "default_open_blockers_output_path",
    "determine_final_exit",
    "display_path",
    "main",
    "normalize_actionable_statuses",
    "open_blockers_refresh_spec",
    "parse_activation_payload",
    "parse_non_negative_int",
    "render_command_log",
    "render_markdown_report",
    "render_spec_lint_log",
    "resolve_repo_path",
    "run_command",
    "run_preflight",
    "snapshot_refresh_spec",
    "spec_lint_spec",
    "write_text",
]


if __name__ == "__main__":
    raise SystemExit(main())
