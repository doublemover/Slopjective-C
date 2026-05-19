"""Activation preflight orchestration flow."""

from __future__ import annotations

import argparse
import sys

from objc3c_tooling.paths import display_path

from scripts.activation_preflight.payload_validation import EXIT_RUNNER_ERROR
from scripts.activation_preflight.reports import REPORT_MD_FILENAME, SUMMARY_JSON_FILENAME
from scripts.activation_preflight.runner_orchestration.artifacts import (
    persist_preflight_artifacts,
)
from scripts.activation_preflight.runner_orchestration.commands import (
    run_activation_commands,
    run_refresh_commands,
)
from scripts.activation_preflight.runner_orchestration.inputs import resolve_preflight_inputs
from scripts.activation_preflight.runner_orchestration.types import CommandRunner
from scripts.activation_preflight.runner_orchestration.validation import (
    validate_activation_results,
)


def run_preflight(args: argparse.Namespace, *, command_runner: CommandRunner) -> int:
    inputs = resolve_preflight_inputs(args)
    errors = list(inputs.initial_errors)

    refresh_results = run_refresh_commands(
        args,
        inputs=inputs,
        command_runner=command_runner,
    )
    errors.extend(refresh_results.errors)

    activation_results = run_activation_commands(
        args,
        inputs=inputs,
        open_blockers_path=refresh_results.open_blockers_path,
        command_runner=command_runner,
    )
    validation = validate_activation_results(
        args,
        inputs=inputs,
        activation_results=activation_results,
        open_blockers_path=refresh_results.open_blockers_path,
        errors=errors,
    )

    try:
        persist_preflight_artifacts(
            args,
            inputs=inputs,
            refresh_results=refresh_results,
            activation_results=activation_results,
            validation=validation,
        )
    except OSError as exc:
        print(f"error: unable to persist preflight artifacts: {exc}", file=sys.stderr)
        return EXIT_RUNNER_ERROR

    print(
        "activation-preflight: "
        f"status={validation.final_status} "
        f"exit_code={validation.final_exit_code} "
        f"summary={display_path(inputs.output_dir / SUMMARY_JSON_FILENAME)} "
        f"report={display_path(inputs.output_dir / REPORT_MD_FILENAME)}"
    )
    return validation.final_exit_code


__all__ = ["run_preflight"]
