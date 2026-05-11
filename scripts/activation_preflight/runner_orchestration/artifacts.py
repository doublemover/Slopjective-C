"""Activation preflight artifact persistence."""

from __future__ import annotations

import argparse
import json
from typing import Any

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
from scripts.activation_preflight.runner_io import write_text
from scripts.activation_preflight.runner_orchestration.commands import (
    ActivationCommandResults,
    RefreshCommandResults,
)
from scripts.activation_preflight.runner_orchestration.inputs import PreflightInputs
from scripts.activation_preflight.runner_orchestration.validation import PreflightValidation


def persist_preflight_artifacts(
    args: argparse.Namespace,
    *,
    inputs: PreflightInputs,
    refresh_results: RefreshCommandResults,
    activation_results: ActivationCommandResults,
    validation: PreflightValidation,
) -> None:
    output_dir = inputs.output_dir
    output_dir.mkdir(parents=True, exist_ok=True)
    activation_json_path = output_dir / ACTIVATION_JSON_FILENAME
    activation_md_path = output_dir / ACTIVATION_MD_FILENAME
    spec_lint_log_path = output_dir / SPEC_LINT_LOG_FILENAME
    snapshot_capture_log_path = output_dir / SNAPSHOT_CAPTURE_LOG_FILENAME
    open_blockers_refresh_log_path = output_dir / OPEN_BLOCKERS_REFRESH_LOG_FILENAME
    summary_json_path = output_dir / SUMMARY_JSON_FILENAME
    report_md_path = output_dir / REPORT_MD_FILENAME

    write_text(activation_json_path, activation_results.activation_json_result.stdout)
    write_text(activation_md_path, activation_results.activation_markdown_result.stdout)
    write_text(spec_lint_log_path, render_spec_lint_log(activation_results.spec_lint_result))
    if refresh_results.snapshot_refresh_result is not None:
        write_text(
            snapshot_capture_log_path,
            render_command_log(
                "capture_activation_snapshots command output",
                refresh_results.snapshot_refresh_result,
            ),
        )
    if refresh_results.open_blockers_refresh_result is not None:
        write_text(
            open_blockers_refresh_log_path,
            render_command_log(
                "extract_open_blockers snapshot-json command output",
                refresh_results.open_blockers_refresh_result,
            ),
        )

    summary: dict[str, Any] = build_summary_payload(
        issues_path=inputs.issues_path,
        milestones_path=inputs.milestones_path,
        catalog_path=inputs.catalog_path,
        open_blockers_path=refresh_results.open_blockers_path,
        output_dir=output_dir,
        spec_globs=tuple(args.spec_globs),
        snapshot_refresh_requested=bool(args.refresh_snapshots),
        snapshot_refresh_result=refresh_results.snapshot_refresh_result,
        open_blockers_refresh_requested=bool(args.refresh_open_blockers),
        open_blockers_refresh_result=refresh_results.open_blockers_refresh_result,
        open_blockers_refresh_root=inputs.open_blockers_refresh_root,
        open_blockers_refresh_generated_at_utc=args.open_blockers_generated_at_utc,
        open_blockers_refresh_source=args.open_blockers_source,
        issues_max_age_seconds=args.issues_max_age_seconds,
        milestones_max_age_seconds=args.milestones_max_age_seconds,
        snapshot_generated_at_utc=args.snapshot_generated_at_utc,
        activation_payload=validation.activation_payload,
        activation_json_result=activation_results.activation_json_result,
        activation_markdown_result=activation_results.activation_markdown_result,
        spec_lint_result=activation_results.spec_lint_result,
        errors=validation.errors,
        final_exit_code=validation.final_exit_code,
        final_status=validation.final_status,
    )
    summary_json = json.dumps(summary, indent=2) + "\n"
    report_markdown = render_markdown_report(summary)

    write_text(summary_json_path, summary_json)
    write_text(report_md_path, report_markdown)


__all__ = ["persist_preflight_artifacts"]
