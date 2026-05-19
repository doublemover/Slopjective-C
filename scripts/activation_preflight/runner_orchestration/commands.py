"""Activation preflight command orchestration."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.paths import display_path

from scripts.activation_preflight.command_specs import (
    activation_check_specs,
    open_blockers_refresh_spec,
    snapshot_refresh_spec,
    spec_lint_spec,
)
from scripts.activation_preflight.contracts import CommandResult
from scripts.activation_preflight.runner_io import write_text
from scripts.activation_preflight.runner_paths import ACTIVATION_CHECK_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import CAPTURE_SNAPSHOTS_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import SPEC_LINT_SCRIPT_PATH
from scripts.activation_preflight.runner_paths import default_open_blockers_output_path
from scripts.activation_preflight.runner_orchestration.inputs import PreflightInputs
from scripts.activation_preflight.runner_orchestration.types import CommandRunner


@dataclass(frozen=True)
class RefreshCommandResults:
    snapshot_refresh_result: CommandResult | None
    open_blockers_refresh_result: CommandResult | None
    open_blockers_path: Path | None
    errors: tuple[str, ...]


@dataclass(frozen=True)
class ActivationCommandResults:
    activation_json_result: CommandResult
    activation_markdown_result: CommandResult
    spec_lint_result: CommandResult


def run_refresh_commands(
    args: argparse.Namespace,
    *,
    inputs: PreflightInputs,
    command_runner: CommandRunner,
) -> RefreshCommandResults:
    errors: list[str] = []
    snapshot_refresh_result: CommandResult | None = None
    open_blockers_refresh_result: CommandResult | None = None
    open_blockers_path = inputs.open_blockers_path

    if args.refresh_snapshots:
        capture_spec = snapshot_refresh_spec(
            script_path=CAPTURE_SNAPSHOTS_SCRIPT_PATH,
            issues_path=inputs.issues_path,
            milestones_path=inputs.milestones_path,
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
            open_blockers_path = default_open_blockers_output_path(inputs.output_dir)

        assert inputs.open_blockers_refresh_root is not None
        extract_spec = open_blockers_refresh_spec(
            script_path=EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
            root=inputs.open_blockers_refresh_root,
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

    return RefreshCommandResults(
        snapshot_refresh_result=snapshot_refresh_result,
        open_blockers_refresh_result=open_blockers_refresh_result,
        open_blockers_path=open_blockers_path,
        errors=tuple(errors),
    )


def run_activation_commands(
    args: argparse.Namespace,
    *,
    inputs: PreflightInputs,
    open_blockers_path: Path | None,
    command_runner: CommandRunner,
) -> ActivationCommandResults:
    activation_json_spec, activation_markdown_spec = activation_check_specs(
        script_path=ACTIVATION_CHECK_SCRIPT_PATH,
        issues_path=inputs.issues_path,
        milestones_path=inputs.milestones_path,
        catalog_path=inputs.catalog_path,
        open_blockers_path=open_blockers_path,
        actionable_statuses=args.actionable_statuses,
        issues_max_age_seconds=args.issues_max_age_seconds,
        milestones_max_age_seconds=args.milestones_max_age_seconds,
        t4_overlay_path=inputs.t4_overlay_path,
        t4_new_scope_publish=bool(args.t4_new_scope_publish),
    )
    spec_lint_command_spec = spec_lint_spec(
        script_path=SPEC_LINT_SCRIPT_PATH,
        spec_globs=args.spec_globs,
    )

    return ActivationCommandResults(
        activation_json_result=command_runner(activation_json_spec),
        activation_markdown_result=command_runner(activation_markdown_spec),
        spec_lint_result=command_runner(spec_lint_command_spec),
    )


__all__ = [
    "ActivationCommandResults",
    "RefreshCommandResults",
    "run_activation_commands",
    "run_refresh_commands",
]
