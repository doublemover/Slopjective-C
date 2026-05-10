"""Activation preflight orchestration flow."""

from __future__ import annotations

import argparse
import json
import sys
from typing import Any, Callable

from objc3c_tooling.paths import display_path, resolve_repo_path
from scripts.activation_preflight.command_specs import (
    activation_check_specs,
    open_blockers_refresh_spec,
    snapshot_refresh_spec,
    spec_lint_spec,
)
from scripts.activation_preflight.contracts import CommandResult, CommandSpec
from scripts.activation_preflight.payload_validation import (
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
from scripts.activation_preflight.runner_io import write_text
from scripts.activation_preflight.runner_paths import ACTIVATION_CHECK_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import CAPTURE_SNAPSHOTS_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import DEFAULT_ACTIONABLE_STATUSES
from scripts.activation_preflight.runner_paths import EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import SPEC_LINT_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import default_open_blockers_output_path
from scripts.activation_preflight.runner_state import determine_final_exit
from scripts.activation_preflight.runner_state import normalize_actionable_statuses

CommandRunner = Callable[[CommandSpec], CommandResult]


def run_preflight(args: argparse.Namespace, *, command_runner: CommandRunner) -> int:
    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json)
        if args.open_blockers_json is not None
        else None
    )
    output_dir = resolve_repo_path(args.output_dir)
    t4_overlay_path = (
        resolve_repo_path(args.t4_governance_overlay_json)
        if args.t4_governance_overlay_json is not None
        else None
    )
    try:
        expected_actionable_statuses = normalize_actionable_statuses(args.actionable_statuses)
    except ValueError as exc:
        expected_actionable_statuses = tuple(
            status for status in (args.actionable_statuses or []) if status
        )
        if not expected_actionable_statuses:
            expected_actionable_statuses = DEFAULT_ACTIONABLE_STATUSES
        errors = [f"invalid actionable-status input: {exc}."]
    else:
        errors: list[str] = []

    snapshot_refresh_result: CommandResult | None = None
    open_blockers_refresh_result: CommandResult | None = None
    open_blockers_refresh_root = (
        resolve_repo_path(args.open_blockers_root) if args.refresh_open_blockers else None
    )

    if args.refresh_snapshots:
        capture_spec = snapshot_refresh_spec(
            script_path=CAPTURE_SNAPSHOTS_SCRIPT_PATH,
            issues_path=issues_path,
            milestones_path=milestones_path,
            generated_at_utc=args.snapshot_generated_at_utc,
        )
        snapshot_refresh_result = command_runner(capture_spec)
        if snapshot_refresh_result.exit_code != 0:
            errors.append(
                "capture_activation_snapshots returned unexpected exit code "
                f"{snapshot_refresh_result.exit_code}."
            )

    if args.refresh_open_blockers:
        if open_blockers_path is None:
            open_blockers_path = default_open_blockers_output_path(output_dir)

        assert open_blockers_refresh_root is not None
        extract_spec = open_blockers_refresh_spec(
            script_path=EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
            root=open_blockers_refresh_root,
            generated_at_utc=args.open_blockers_generated_at_utc,
            source=args.open_blockers_source,
        )
        open_blockers_refresh_result = command_runner(extract_spec)
        if open_blockers_refresh_result.exit_code != 0:
            errors.append(
                "extract_open_blockers(snapshot-json) returned unexpected exit code "
                f"{open_blockers_refresh_result.exit_code}."
            )
        else:
            try:
                write_text(open_blockers_path, open_blockers_refresh_result.stdout)
            except OSError as exc:
                errors.append(
                    "unable to persist refreshed open blockers snapshot to "
                    f"{display_path(open_blockers_path)}: {exc}."
                )

    activation_json_spec, activation_markdown_spec = activation_check_specs(
        script_path=ACTIVATION_CHECK_SCRIPT_PATH,
        issues_path=issues_path,
        milestones_path=milestones_path,
        catalog_path=catalog_path,
        open_blockers_path=open_blockers_path,
        actionable_statuses=args.actionable_statuses,
        issues_max_age_seconds=args.issues_max_age_seconds,
        milestones_max_age_seconds=args.milestones_max_age_seconds,
        t4_overlay_path=t4_overlay_path,
        t4_new_scope_publish=bool(args.t4_new_scope_publish),
    )
    spec_lint_command_spec = spec_lint_spec(
        script_path=SPEC_LINT_SCRIPT_PATH,
        spec_globs=args.spec_globs,
    )

    activation_json_result = command_runner(activation_json_spec)
    activation_markdown_result = command_runner(activation_markdown_spec)
    spec_lint_result = command_runner(spec_lint_command_spec)

    activation_payload, activation_error = parse_activation_payload(
        activation_json_result,
        expected_issues_path=issues_path,
        expected_milestones_path=milestones_path,
        expected_catalog_path=catalog_path,
        expected_open_blockers_path=open_blockers_path,
        expected_t4_overlay_path=t4_overlay_path,
        expected_actionable_statuses=expected_actionable_statuses,
        expected_issues_max_age_seconds=args.issues_max_age_seconds,
        expected_milestones_max_age_seconds=args.milestones_max_age_seconds,
    )
    if activation_error is not None:
        errors.append(activation_error)

    if activation_payload is not None:
        markdown_error = check_markdown_gate_consistency(
            activation_markdown_result,
            activation_payload=activation_payload,
        )
        if markdown_error is not None:
            errors.append(markdown_error)

    final_exit_code, final_status = determine_final_exit(
        activation_payload=activation_payload,
        spec_lint_exit_code=spec_lint_result.exit_code,
        errors=errors,
    )

    try:
        output_dir.mkdir(parents=True, exist_ok=True)
        activation_json_path = output_dir / ACTIVATION_JSON_FILENAME
        activation_md_path = output_dir / ACTIVATION_MD_FILENAME
        spec_lint_log_path = output_dir / SPEC_LINT_LOG_FILENAME
        snapshot_capture_log_path = output_dir / SNAPSHOT_CAPTURE_LOG_FILENAME
        open_blockers_refresh_log_path = output_dir / OPEN_BLOCKERS_REFRESH_LOG_FILENAME
        summary_json_path = output_dir / SUMMARY_JSON_FILENAME
        report_md_path = output_dir / REPORT_MD_FILENAME

        write_text(activation_json_path, activation_json_result.stdout)
        write_text(activation_md_path, activation_markdown_result.stdout)
        write_text(spec_lint_log_path, render_spec_lint_log(spec_lint_result))
        if snapshot_refresh_result is not None:
            write_text(
                snapshot_capture_log_path,
                render_command_log(
                    "capture_activation_snapshots command output",
                    snapshot_refresh_result,
                ),
            )
        if open_blockers_refresh_result is not None:
            write_text(
                open_blockers_refresh_log_path,
                render_command_log(
                    "extract_open_blockers snapshot-json command output",
                    open_blockers_refresh_result,
                ),
            )

        summary: dict[str, Any] = build_summary_payload(
            issues_path=issues_path,
            milestones_path=milestones_path,
            catalog_path=catalog_path,
            open_blockers_path=open_blockers_path,
            output_dir=output_dir,
            spec_globs=tuple(args.spec_globs),
            snapshot_refresh_requested=bool(args.refresh_snapshots),
            snapshot_refresh_result=snapshot_refresh_result,
            open_blockers_refresh_requested=bool(args.refresh_open_blockers),
            open_blockers_refresh_result=open_blockers_refresh_result,
            open_blockers_refresh_root=open_blockers_refresh_root,
            open_blockers_refresh_generated_at_utc=args.open_blockers_generated_at_utc,
            open_blockers_refresh_source=args.open_blockers_source,
            issues_max_age_seconds=args.issues_max_age_seconds,
            milestones_max_age_seconds=args.milestones_max_age_seconds,
            snapshot_generated_at_utc=args.snapshot_generated_at_utc,
            activation_payload=activation_payload,
            activation_json_result=activation_json_result,
            activation_markdown_result=activation_markdown_result,
            spec_lint_result=spec_lint_result,
            errors=tuple(errors),
            final_exit_code=final_exit_code,
            final_status=final_status,
        )
        summary_json = json.dumps(summary, indent=2) + "\n"
        report_markdown = render_markdown_report(summary)

        write_text(summary_json_path, summary_json)
        write_text(report_md_path, report_markdown)
    except OSError as exc:
        print(f"error: unable to persist preflight artifacts: {exc}", file=sys.stderr)
        return EXIT_RUNNER_ERROR

    print(
        "activation-preflight: "
        f"status={final_status} "
        f"exit_code={final_exit_code} "
        f"summary={display_path(output_dir / SUMMARY_JSON_FILENAME)} "
        f"report={display_path(output_dir / REPORT_MD_FILENAME)}"
    )
    return final_exit_code


__all__ = ["CommandRunner", "run_preflight"]
