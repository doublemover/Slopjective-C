"""Runner support facade for deterministic open-blocker audit orchestration."""

from __future__ import annotations

from .audit_runner_commands import run_command, summarize_command
from .audit_runner_constants import (
    CHECKER_MODE,
    CONTRACT_CHECK_STDERR_FILENAME,
    CONTRACT_CHECK_TRANSCRIPT_FILENAME,
    DEFAULT_COMMAND_TIMEOUT_SECONDS,
    DEFAULT_SNAPSHOT_RELATIVE_PATH,
    EXIT_OK,
    EXIT_OPEN_BLOCKERS,
    EXIT_RUNNER_ERROR,
    EXTRACT_LOG_FILENAME,
    REPORT_MD_FILENAME,
    RUNNER_CONTRACT_ID,
    RUNNER_CONTRACT_VERSION,
    RUNNER_ID,
    SUMMARY_JSON_FILENAME,
)
from .audit_runner_io import bool_text, write_text
from .audit_runner_models import CommandResult, CommandSpec
from .audit_runner_payloads import (
    build_runner_snapshot_payload,
    build_summary_payload,
    determine_final_exit,
)
from .audit_runner_rendering import (
    render_command_log,
    render_contract_check_transcript,
    render_markdown_report,
)


__all__ = [
    "CHECKER_MODE",
    "CONTRACT_CHECK_STDERR_FILENAME",
    "CONTRACT_CHECK_TRANSCRIPT_FILENAME",
    "DEFAULT_COMMAND_TIMEOUT_SECONDS",
    "DEFAULT_SNAPSHOT_RELATIVE_PATH",
    "EXIT_OK",
    "EXIT_OPEN_BLOCKERS",
    "EXIT_RUNNER_ERROR",
    "EXTRACT_LOG_FILENAME",
    "REPORT_MD_FILENAME",
    "RUNNER_CONTRACT_ID",
    "RUNNER_CONTRACT_VERSION",
    "RUNNER_ID",
    "SUMMARY_JSON_FILENAME",
    "CommandResult",
    "CommandSpec",
    "bool_text",
    "build_runner_snapshot_payload",
    "build_summary_payload",
    "determine_final_exit",
    "render_command_log",
    "render_contract_check_transcript",
    "render_markdown_report",
    "run_command",
    "summarize_command",
    "write_text",
]
