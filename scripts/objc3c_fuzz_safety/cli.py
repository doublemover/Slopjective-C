"""CLI parsing and entrypoint orchestration for the objc3c fuzz-safety runner."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from objc3c_tooling.paths import display_path

from .aggregation import evaluate
from .config import (
    DEFAULT_COMPILER,
    DEFAULT_MANIFEST,
    DEFAULT_OUT_ROOT,
    EXIT_GUARD_FAIL,
    EXIT_INPUT_ERROR,
    EXIT_OK,
    InputError,
    parse_generated_at_utc,
    validate_inputs,
)
from .reporting import canonical_json_text, render_human_summary


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_objc3c_fuzz_safety.py",
        description=(
            "Deterministic malformed-input stress/fuzz gate for objc3c parser "
            "and semantic passes."
        ),
    )
    parser.add_argument(
        "--compiler",
        type=Path,
        default=DEFAULT_COMPILER,
        help=f"Path to compiler executable (default: {display_path(DEFAULT_COMPILER)}).",
    )
    parser.add_argument(
        "--python-launcher",
        type=Path,
        help="Optional launcher used to run --compiler (useful for test harnesses).",
    )
    parser.add_argument(
        "--out-root",
        type=Path,
        default=DEFAULT_OUT_ROOT,
        help=f"Output root for corpus/log artifacts (default: {display_path(DEFAULT_OUT_ROOT)}).",
    )
    parser.add_argument(
        "--manifest",
        type=Path,
        default=DEFAULT_MANIFEST,
        help=f"Checked-in parser/sema fuzz manifest (default: {display_path(DEFAULT_MANIFEST)}).",
    )
    parser.add_argument(
        "--timeout-sec",
        type=float,
        default=5.0,
        help="Per-invocation timeout in seconds (default: 5.0).",
    )
    parser.add_argument(
        "--max-cases",
        type=int,
        help="Optional deterministic cap for corpus size.",
    )
    parser.add_argument(
        "--generated-at-utc",
        help="Optional deterministic timestamp in UTC format YYYY-MM-DDTHH:MM:SSZ.",
    )
    parser.add_argument(
        "--contract-mode",
        action="store_true",
        help="Emit deterministic JSON summary to stdout.",
    )
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        validate_inputs(args)
        generated_at_utc = parse_generated_at_utc(args.generated_at_utc)
    except InputError as exc:
        print(f"objc3c-fuzz-safety: error: {exc}", file=sys.stderr)
        return EXIT_INPUT_ERROR

    compiler_command: list[str]
    if args.python_launcher is not None:
        compiler_command = [str(args.python_launcher.resolve()), str(args.compiler.resolve())]
    else:
        compiler_command = [str(args.compiler.resolve())]

    try:
        summary = evaluate(
            compiler_command=compiler_command,
            out_root=args.out_root.resolve(),
            manifest_path=args.manifest.resolve(),
            timeout_sec=float(args.timeout_sec),
            max_cases=args.max_cases,
            generated_at_utc=generated_at_utc,
        )
    except OSError as exc:
        print(f"objc3c-fuzz-safety: error: failed to execute compiler: {exc}", file=sys.stderr)
        return EXIT_INPUT_ERROR

    if args.contract_mode:
        print(canonical_json_text(summary), end="")
    elif summary["status"] == "FAIL":
        print(render_human_summary(summary), file=sys.stderr, end="")
    else:
        print(render_human_summary(summary), end="")

    return EXIT_OK if summary["status"] == "PASS" else EXIT_GUARD_FAIL
