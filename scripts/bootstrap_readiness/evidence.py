"""Evidence artifact persistence for bootstrap readiness orchestration."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.json_io import write_text_file
from objc3c_tooling.public_workflow_output import normalize_newlines

from .models import CommandResult
from .reports import (
    BOOTSTRAP_JSON_FILENAME,
    BOOTSTRAP_JSON_LOG_FILENAME,
    BOOTSTRAP_MD_FILENAME,
    BOOTSTRAP_MD_LOG_FILENAME,
    OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
    REPORT_MD_FILENAME,
    SPEC_LINT_LOG_FILENAME,
    SUMMARY_JSON_FILENAME,
    build_summary_payload,
    render_command_log,
    render_markdown_report,
)


def write_text(path: Path, content: str) -> None:
    write_text_file(path, normalize_newlines(content))


def persist_artifacts(
    *,
    output_dir: Path,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_path: Path | None,
    refresh_open_blockers_requested: bool,
    refresh_open_blockers_result: CommandResult | None,
    refresh_open_blockers_root: Path | None,
    refresh_open_blockers_generated_at_utc: str | None,
    refresh_open_blockers_source: str | None,
    checker_json_result: CommandResult,
    checker_markdown_result: CommandResult,
    checker_payload: dict[str, Any] | None,
    run_spec_lint_requested: bool,
    spec_globs: Sequence[str],
    spec_lint_result: CommandResult | None,
    errors: Sequence[str],
    final_status: str,
    final_exit_code: int,
) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)

    checker_json_path = output_dir / BOOTSTRAP_JSON_FILENAME
    checker_md_path = output_dir / BOOTSTRAP_MD_FILENAME
    checker_json_log_path = output_dir / BOOTSTRAP_JSON_LOG_FILENAME
    checker_md_log_path = output_dir / BOOTSTRAP_MD_LOG_FILENAME
    refresh_log_path = output_dir / OPEN_BLOCKERS_REFRESH_LOG_FILENAME
    spec_lint_log_path = output_dir / SPEC_LINT_LOG_FILENAME
    summary_json_path = output_dir / SUMMARY_JSON_FILENAME
    report_md_path = output_dir / REPORT_MD_FILENAME

    write_text(checker_json_path, checker_json_result.stdout)
    write_text(checker_md_path, checker_markdown_result.stdout)
    write_text(
        checker_json_log_path,
        render_command_log(
            "check_bootstrap_readiness json command output",
            checker_json_result,
        ),
    )
    write_text(
        checker_md_log_path,
        render_command_log(
            "check_bootstrap_readiness markdown command output",
            checker_markdown_result,
        ),
    )
    if refresh_open_blockers_result is not None:
        write_text(
            refresh_log_path,
            render_command_log(
                "extract_open_blockers snapshot-json command output",
                refresh_open_blockers_result,
            ),
        )
    if spec_lint_result is not None:
        write_text(
            spec_lint_log_path,
            render_command_log("spec_lint command output", spec_lint_result),
        )

    summary = build_summary_payload(
        issues_path=issues_path,
        milestones_path=milestones_path,
        catalog_path=catalog_path,
        open_blockers_path=open_blockers_path,
        output_dir=output_dir,
        refresh_open_blockers_requested=refresh_open_blockers_requested,
        refresh_open_blockers_result=refresh_open_blockers_result,
        refresh_open_blockers_root=refresh_open_blockers_root,
        refresh_open_blockers_generated_at_utc=refresh_open_blockers_generated_at_utc,
        refresh_open_blockers_source=refresh_open_blockers_source,
        checker_json_result=checker_json_result,
        checker_markdown_result=checker_markdown_result,
        checker_payload=checker_payload,
        run_spec_lint_requested=run_spec_lint_requested,
        spec_globs=tuple(spec_globs),
        spec_lint_result=spec_lint_result,
        errors=tuple(errors),
        final_status=final_status,
        final_exit_code=final_exit_code,
    )
    summary_json = json.dumps(summary, indent=2) + "\n"
    report_markdown = render_markdown_report(summary)

    write_text(summary_json_path, summary_json)
    write_text(report_md_path, report_markdown)
