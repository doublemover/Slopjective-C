from __future__ import annotations

import argparse
import json
import sys
from typing import Any, Sequence

from objc3c_tooling.json_io import canonical_json, write_json_file as write_json
from objc3c_tooling.paths import display_path

from objc3c_library_cli_parity.artifacts import sha256_text
from objc3c_library_cli_parity.contracts import MODE
from objc3c_library_cli_parity.contracts import ParityInputs
from objc3c_library_cli_parity.fixtures import (
    synthetic_fixture_contract_payload,
    synthetic_fixture_summary_envelope,
    validate_synthetic_fixture_contract,
)


def build_summary(
    *,
    inputs: ParityInputs,
    artifacts: Sequence[str],
    dimensions: list[dict[str, str]],
    comparisons: list[dict[str, Any]],
    failures: list[str],
) -> dict[str, Any]:
    return {
        "mode": MODE,
        "library_dir": display_path(inputs.library_dir),
        "cli_dir": display_path(inputs.cli_dir),
        "artifacts": list(artifacts),
        "dimensions": dimensions,
        "comparisons": comparisons,
        "failures": failures,
        "ok": not failures,
    }


def apply_synthetic_fixture_contract(
    summary: dict[str, Any],
    *,
    inputs: ParityInputs,
    failures: list[str],
) -> None:
    synthetic_contract_applied, synthetic_failures, synthetic_details = (
        validate_synthetic_fixture_contract(
            library_dir=inputs.library_dir,
            cli_dir=inputs.cli_dir,
        )
    )
    if synthetic_contract_applied:
        summary["artifact_authenticity"] = synthetic_fixture_summary_envelope()
        summary["synthetic_fixture_contract"] = synthetic_fixture_contract_payload()
        summary["authenticity_checks"] = {
            **(synthetic_details or {}),
            "failure_count": len(synthetic_failures),
        }
        failures.extend(synthetic_failures)


def apply_source_execution_summary(
    summary: dict[str, Any],
    *,
    args: argparse.Namespace,
    inputs: ParityInputs,
) -> None:
    if inputs.execution is None:
        return
    summary["execution"] = {
        "source": display_path(args.source),
        "emit_prefix": args.emit_prefix,
        "work_key": inputs.execution.work_key,
        "routing": inputs.execution.routing,
        "commands": [
            {
                "role": result.role,
                "command": result.command,
                "exit_code": result.exit_code,
                "stdout": result.stdout,
                "stderr": result.stderr,
            }
            for result in inputs.execution.results
        ],
    }


def apply_golden_summary(
    summary: dict[str, Any],
    *,
    args: argparse.Namespace,
    failures: list[str],
) -> None:
    if args.golden_summary is None:
        return

    golden_path = args.golden_summary
    summary["golden_summary"] = display_path(golden_path)
    if args.write_golden:
        write_json(golden_path, summary)
        print(f"golden-updated: {display_path(golden_path)}")
    elif args.check_golden:
        if not golden_path.exists():
            failures.append(
                "golden summary missing: "
                f"{display_path(golden_path)} (run with --write-golden to create it)"
            )
        else:
            try:
                expected = json.loads(golden_path.read_text(encoding="utf-8"))
            except json.JSONDecodeError as exc:
                failures.append(
                    f"golden summary parse error at {display_path(golden_path)}: {exc}"
                )
            else:
                if expected != summary:
                    observed_digest = sha256_text(canonical_json(summary))
                    expected_digest = sha256_text(canonical_json(expected))
                    failures.append(
                        "golden summary drift detected: "
                        f"expected_sha256={expected_digest[:16]} "
                        f"observed_sha256={observed_digest[:16]} "
                        f"(update with --write-golden after intended contract changes)"
                    )


def write_summary_and_report(
    *,
    args: argparse.Namespace,
    summary: dict[str, Any],
    failures: list[str],
    comparisons: list[dict[str, Any]],
    dimensions: list[dict[str, str]],
) -> int:
    summary["ok"] = not failures
    write_json(args.summary_out, summary)

    if failures:
        for failure in failures:
            print(f"PARITY-FAIL: {failure}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    compared_dimensions = sum(
        1 for item in dimensions if item["status"] == "compared"
    )
    print(
        "PARITY-PASS: "
        f"compared {len(comparisons)} artifact(s), dimensions={compared_dimensions}"
    )
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


__all__ = [
    "apply_golden_summary",
    "apply_source_execution_summary",
    "apply_synthetic_fixture_contract",
    "build_summary",
    "write_summary_and_report",
]
