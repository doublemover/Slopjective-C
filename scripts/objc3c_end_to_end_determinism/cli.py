"""Command-line orchestration for the end-to-end determinism checker."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path
from scripts.objc3c_end_to_end_determinism.commands import (
    normalize_command_tokens,
    parse_key_value,
    parse_variant_env,
    run_once,
)
from scripts.objc3c_end_to_end_determinism.comparison import compare_runs
from scripts.objc3c_end_to_end_determinism.constants import DEFAULT_REPLAY_ROOT, ROOT
from scripts.objc3c_end_to_end_determinism.errors import DeterminismContractError
from scripts.objc3c_end_to_end_determinism.reports import (
    build_summary_payload,
    emit_failure_report,
    emit_success_report,
    write_summary_payload,
)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_objc3c_end_to_end_determinism.py",
        description=(
            "Replay command execution and compare artifact digests across runs "
            "to enforce end-to-end determinism."
        ),
    )
    parser.add_argument(
        "--replays",
        type=int,
        default=2,
        help="Number of replay runs to execute (default: 2).",
    )
    parser.add_argument(
        "--artifact-glob",
        action="append",
        dest="artifact_globs",
        default=[],
        help=(
            "Artifact glob pattern relative to each run directory. "
            "May be passed multiple times. Defaults to '**/*' when omitted."
        ),
    )
    parser.add_argument(
        "--workdir",
        type=Path,
        default=ROOT,
        help=f"Working directory for replay command (default: {display_path(ROOT)}).",
    )
    parser.add_argument(
        "--replay-root",
        type=Path,
        default=DEFAULT_REPLAY_ROOT,
        help=(
            "Root directory for per-run replay artifacts "
            f"(default: {display_path(DEFAULT_REPLAY_ROOT)})."
        ),
    )
    parser.add_argument(
        "--run-label",
        default="latest",
        help="Replay session label under replay-root (default: latest).",
    )
    parser.add_argument(
        "--summary-json",
        type=Path,
        default=None,
        help="Optional path for digest evidence JSON summary.",
    )
    parser.add_argument(
        "--env",
        action="append",
        default=[],
        metavar="KEY=VALUE",
        help="Static env override applied to every replay run.",
    )
    parser.add_argument(
        "--variant-env",
        action="append",
        default=[],
        metavar="KEY=v1,v2,...",
        help="Per-run env override values; value count must equal --replays.",
    )
    parser.add_argument(
        "command",
        nargs=argparse.REMAINDER,
        help=(
            "Command tokens after '--'. Supports {repo_root}, {run_dir}, {run_id} "
            "placeholders."
        ),
    )
    return parser


def check_determinism(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    if args.replays < 2:
        raise DeterminismContractError("--replays must be >= 2")

    command_template = normalize_command_tokens(args.command)
    artifact_globs = tuple(args.artifact_globs) if args.artifact_globs else ("**/*",)
    workdir = args.workdir.resolve()
    replay_root = args.replay_root.resolve()
    run_label = str(args.run_label)
    if not run_label:
        raise DeterminismContractError("--run-label must be non-empty")

    base_env = parse_key_value(args.env, context="--env")
    variant_env = parse_variant_env(args.variant_env, replays=args.replays)

    session_dir = replay_root / run_label
    session_dir.mkdir(parents=True, exist_ok=True)
    summary_json = (
        args.summary_json.resolve() if args.summary_json else (session_dir / "summary.json")
    )
    summary_json.parent.mkdir(parents=True, exist_ok=True)

    runs: list[dict[str, object]] = []
    command_failures: list[dict[str, object]] = []
    for index in range(args.replays):
        run_id = f"run{index + 1:02d}"
        run_dir = session_dir / run_id
        run_record = run_once(
            command_template=command_template,
            run_dir=run_dir,
            run_label=run_id,
            workdir=workdir,
            base_env=base_env,
            variant_env=variant_env,
            replay_index=index,
            artifact_globs=artifact_globs,
        )
        runs.append(run_record)
        if int(run_record["exit_code"]) != 0:
            command_failures.append(
                {
                    "kind": "command-exit-nonzero",
                    "run_id": run_id,
                    "exit_code": run_record["exit_code"],
                }
            )

    mismatches = compare_runs(runs)
    failures = command_failures + mismatches

    payload = build_summary_payload(
        replays=args.replays,
        artifact_globs=artifact_globs,
        command_template=command_template,
        workdir=workdir,
        replay_root=replay_root,
        run_label=run_label,
        session_dir=session_dir,
        summary_json=summary_json,
        runs=runs,
        failures=failures,
    )
    write_summary_payload(summary_json=summary_json, payload=payload)

    if failures:
        emit_failure_report(
            replays=args.replays,
            failures=failures,
            replay_root=replay_root,
            run_label=run_label,
            command_template=command_template,
            summary_json=summary_json,
        )
        return 1

    emit_success_report(
        replays=args.replays,
        artifact_count=int(runs[0]["artifact_count"]),
        summary_json=summary_json,
    )
    return 0


def main(argv: Sequence[str] | None = None) -> int:
    try:
        return check_determinism(argv)
    except DeterminismContractError as exc:
        print(f"objc3c-end-to-end-determinism: error: {exc}", file=sys.stderr)
        return 1


__all__ = ["build_parser", "check_determinism", "main"]
