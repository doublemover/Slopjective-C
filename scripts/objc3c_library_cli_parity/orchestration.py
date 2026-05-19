from __future__ import annotations

import argparse
from typing import Sequence

from objc3c_library_cli_parity.arguments import (
    normalize_cli_config,
    parse_args,
    validate_golden_options,
)
from objc3c_library_cli_parity.artifacts import normalize_artifacts, parse_dimension_map
from objc3c_library_cli_parity.comparison import evaluate_parity
from objc3c_library_cli_parity.inputs import load_parity_inputs
from objc3c_library_cli_parity.reporting import (
    apply_golden_summary,
    apply_source_execution_summary,
    apply_synthetic_fixture_contract,
    build_summary,
    write_summary_and_report,
)


def run_from_args(args: argparse.Namespace) -> int:
    validate_golden_options(args)
    normalize_cli_config(args)

    inputs = load_parity_inputs(args)
    artifacts = normalize_artifacts(args.artifacts or inputs.default_artifacts)
    dimension_map = parse_dimension_map(
        args.dimension_map,
        default_mapping=inputs.default_dimension_map,
    )

    evaluation = evaluate_parity(
        inputs=inputs,
        artifacts=artifacts,
        dimension_map=dimension_map,
    )
    failures = list(inputs.execution.failures if inputs.execution is not None else [])
    failures.extend(evaluation.failures)

    summary = build_summary(
        inputs=inputs,
        artifacts=artifacts,
        dimensions=evaluation.dimensions,
        comparisons=evaluation.comparisons,
        failures=failures,
    )
    apply_synthetic_fixture_contract(
        summary,
        inputs=inputs,
        failures=failures,
    )
    apply_source_execution_summary(
        summary,
        args=args,
        inputs=inputs,
    )
    apply_golden_summary(
        summary,
        args=args,
        failures=failures,
    )
    return write_summary_and_report(
        args=args,
        summary=summary,
        failures=failures,
        comparisons=evaluation.comparisons,
        dimensions=evaluation.dimensions,
    )


def run(argv: Sequence[str]) -> int:
    return run_from_args(parse_args(argv))


__all__ = ["run", "run_from_args"]
