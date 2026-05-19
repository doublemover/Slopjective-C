"""Activation preflight result validation."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

from scripts.activation_preflight.payload_validation import (
    check_markdown_gate_consistency,
    parse_activation_payload,
)
from scripts.activation_preflight.runner_state import determine_final_exit
from scripts.activation_preflight.runner_orchestration.commands import ActivationCommandResults
from scripts.activation_preflight.runner_orchestration.inputs import PreflightInputs


@dataclass(frozen=True)
class PreflightValidation:
    activation_payload: dict[str, Any] | None
    errors: tuple[str, ...]
    final_exit_code: int
    final_status: str


def validate_activation_results(
    args: argparse.Namespace,
    *,
    inputs: PreflightInputs,
    activation_results: ActivationCommandResults,
    open_blockers_path: Path | None,
    errors: Sequence[str],
) -> PreflightValidation:
    collected_errors = list(errors)
    activation_payload, activation_error = parse_activation_payload(
        activation_results.activation_json_result,
        expected_issues_path=inputs.issues_path,
        expected_milestones_path=inputs.milestones_path,
        expected_catalog_path=inputs.catalog_path,
        expected_open_blockers_path=open_blockers_path,
        expected_t4_overlay_path=inputs.t4_overlay_path,
        expected_actionable_statuses=inputs.expected_actionable_statuses,
        expected_issues_max_age_seconds=args.issues_max_age_seconds,
        expected_milestones_max_age_seconds=args.milestones_max_age_seconds,
    )
    if activation_error is not None:
        collected_errors.append(activation_error)

    if activation_payload is not None:
        markdown_error = check_markdown_gate_consistency(
            activation_results.activation_markdown_result,
            activation_payload=activation_payload,
        )
        if markdown_error is not None:
            collected_errors.append(markdown_error)

    final_exit_code, final_status = determine_final_exit(
        activation_payload=activation_payload,
        spec_lint_exit_code=activation_results.spec_lint_result.exit_code,
        errors=collected_errors,
    )

    return PreflightValidation(
        activation_payload=activation_payload,
        errors=tuple(collected_errors),
        final_exit_code=final_exit_code,
        final_status=final_status,
    )


__all__ = ["PreflightValidation", "validate_activation_results"]
