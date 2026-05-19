#!/usr/bin/env python3
"""Run deterministic repo-root open blocker audit orchestration."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from scripts.open_blocker_audit import (  # noqa: E402
    CHECKER_MODE,
    CHECK_OPEN_BLOCKER_AUDIT_CONTRACT_SCRIPT_PATH,
    CONTRACT_CHECK_STDERR_FILENAME,
    CONTRACT_CHECK_TRANSCRIPT_FILENAME,
    DEFAULT_AUDIT_ROOT,
    DEFAULT_COMMAND_TIMEOUT_SECONDS,
    DEFAULT_EXCLUDE_PATHS,
    DEFAULT_OUTPUT_DIR,
    DEFAULT_SNAPSHOT_RELATIVE_PATH,
    EXIT_OK,
    EXIT_OPEN_BLOCKERS,
    EXIT_RUNNER_ERROR,
    EXTRACT_LOG_FILENAME,
    EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
    REPORT_MD_FILENAME,
    RUNNER_CONTRACT_ID,
    RUNNER_CONTRACT_VERSION,
    RUNNER_ID,
    SUMMARY_JSON_FILENAME,
    AuditOutputPaths,
    AuditScope,
    CommandResult,
    CommandSpec,
    SnapshotMetadata,
    build_extract_snapshot_spec,
    build_extractor_exclude_paths,
    build_output_paths,
    build_parser,
    build_runner_snapshot_payload,
    build_summary_payload,
    build_contract_check_spec,
    determine_final_exit,
    extract_blocker_count,
    normalize_exclude_paths,
    normalize_extract_snapshot_stdout,
    normalize_include_globs,
    normalize_snapshot_metadata,
    render_command_log,
    render_contract_check_transcript,
    render_markdown_report,
    resolve_audit_scope,
    resolve_effective_audit_root,
    resolve_markdown_scope,
    run_audit,
    run_cli,
    run_command,
    validate_contract_check_output,
    validate_extract_snapshot_payload,
    validate_generated_at_utc,
    validate_snapshot_source,
    write_text,
)
from objc3c_tooling.paths import display_path, resolve_repo_path  # noqa: E402


def main(argv: Sequence[str] | None = None) -> int:
    return run_cli(argv, command_runner=run_command)


__all__ = [
    "CHECKER_MODE",
    "CHECK_OPEN_BLOCKER_AUDIT_CONTRACT_SCRIPT_PATH",
    "CONTRACT_CHECK_STDERR_FILENAME",
    "CONTRACT_CHECK_TRANSCRIPT_FILENAME",
    "DEFAULT_AUDIT_ROOT",
    "DEFAULT_COMMAND_TIMEOUT_SECONDS",
    "DEFAULT_EXCLUDE_PATHS",
    "DEFAULT_OUTPUT_DIR",
    "DEFAULT_SNAPSHOT_RELATIVE_PATH",
    "EXIT_OK",
    "EXIT_OPEN_BLOCKERS",
    "EXIT_RUNNER_ERROR",
    "EXTRACT_LOG_FILENAME",
    "EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH",
    "REPORT_MD_FILENAME",
    "ROOT",
    "RUNNER_CONTRACT_ID",
    "RUNNER_CONTRACT_VERSION",
    "RUNNER_ID",
    "SUMMARY_JSON_FILENAME",
    "AuditOutputPaths",
    "AuditScope",
    "CommandResult",
    "CommandSpec",
    "SnapshotMetadata",
    "build_extract_snapshot_spec",
    "build_extractor_exclude_paths",
    "build_output_paths",
    "build_parser",
    "build_runner_snapshot_payload",
    "build_summary_payload",
    "build_contract_check_spec",
    "determine_final_exit",
    "display_path",
    "extract_blocker_count",
    "main",
    "normalize_exclude_paths",
    "normalize_extract_snapshot_stdout",
    "normalize_include_globs",
    "normalize_snapshot_metadata",
    "render_command_log",
    "render_contract_check_transcript",
    "render_markdown_report",
    "resolve_audit_scope",
    "resolve_effective_audit_root",
    "resolve_markdown_scope",
    "resolve_repo_path",
    "run_audit",
    "run_cli",
    "run_command",
    "validate_contract_check_output",
    "validate_extract_snapshot_payload",
    "validate_generated_at_utc",
    "validate_snapshot_source",
    "write_text",
]


if __name__ == "__main__":
    raise SystemExit(main())
