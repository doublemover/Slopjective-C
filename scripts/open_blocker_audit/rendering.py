"""Open-blocker audit artifact rendering."""

from __future__ import annotations

from pathlib import Path

from scripts.open_blocker_extraction.audit_contract import (
    validate_contract_check_result as validate_contract_check_output,
)
from scripts.open_blocker_extraction.audit_runner import (
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
    build_summary_payload,
    determine_final_exit,
    render_command_log,
    render_contract_check_transcript,
    render_markdown_report,
    write_text,
)
from objc3c_tooling.paths import display_path

from .constants import CHECK_OPEN_BLOCKER_AUDIT_CONTRACT_SCRIPT_PATH
from .models import AuditOutputPaths, CommandSpec


def build_output_paths(*, output_dir: Path, snapshot_json_path: Path) -> AuditOutputPaths:
    return AuditOutputPaths(
        output_dir=output_dir,
        snapshot_json_path=snapshot_json_path,
        extract_log_path=output_dir / EXTRACT_LOG_FILENAME,
        summary_json_path=output_dir / SUMMARY_JSON_FILENAME,
        report_md_path=output_dir / REPORT_MD_FILENAME,
        contract_check_transcript_path=output_dir / CONTRACT_CHECK_TRANSCRIPT_FILENAME,
        contract_check_stderr_path=output_dir / CONTRACT_CHECK_STDERR_FILENAME,
    )


def build_contract_check_spec(paths: AuditOutputPaths) -> CommandSpec:
    return CommandSpec(
        name="check_open_blocker_audit_contract",
        script_path=CHECK_OPEN_BLOCKER_AUDIT_CONTRACT_SCRIPT_PATH,
        actual_args=(
            "--summary",
            str(paths.summary_json_path),
            "--snapshot",
            str(paths.snapshot_json_path),
            "--extract-log",
            str(paths.extract_log_path),
            "--contract-id",
            RUNNER_CONTRACT_ID,
            "--contract-version",
            RUNNER_CONTRACT_VERSION,
        ),
        display_args=(
            "--summary",
            display_path(paths.summary_json_path),
            "--snapshot",
            display_path(paths.snapshot_json_path),
            "--extract-log",
            display_path(paths.extract_log_path),
            "--contract-id",
            RUNNER_CONTRACT_ID,
            "--contract-version",
            RUNNER_CONTRACT_VERSION,
        ),
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
    "build_contract_check_spec",
    "build_output_paths",
    "build_summary_payload",
    "determine_final_exit",
    "render_command_log",
    "render_contract_check_transcript",
    "render_markdown_report",
    "validate_contract_check_output",
    "write_text",
]
