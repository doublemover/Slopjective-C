"""CLI parsing and entrypoint orchestration for comparative baseline runs."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.json_io import write_json_file as write_json

from .catalog import load_run_config
from .paths import SUMMARY_OUT
from .reporting import build_summary_payload, render_console_result
from .runner import run_comparative_baselines


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Run the checked-in ObjC2, Swift, and C++ comparative baseline workloads."
    )
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--warmup-runs", type=int, default=None)
    parser.add_argument("--measured-runs", type=int, default=None)
    return parser


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    return build_arg_parser().parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    config = load_run_config(
        warmup_runs_override=args.warmup_runs,
        measured_runs_override=args.measured_runs,
    )
    result = run_comparative_baselines(config)
    payload = build_summary_payload(
        config=config,
        packet_paths=result.packet_paths,
        failures=result.failures,
    )
    write_json(args.summary_out, payload)
    return render_console_result(summary_out=args.summary_out, failures=result.failures)
