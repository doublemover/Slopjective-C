"""LLVM capability probe workflow helpers."""

from __future__ import annotations

import argparse
import subprocess
import sys

from objc3c_tooling.json_io import load_json_object as load_json

from ..commands import run
from ..environment import ROOT
from .developer_tooling_llvm_contracts import (
    default_llvm_capabilities_command,
    capability_probe_command,
    hosted_llvm_probe_command,
)
from .developer_tooling_paths import (
    HOSTED_LLVM_CAPABILITIES_SUMMARY,
)


def action_check_llvm_capabilities(rest: list[str]) -> int:
    parser = argparse.ArgumentParser(prog="npm run objc3c -- check-llvm-capabilities")
    parser.add_argument("--summary-out")
    args = parser.parse_args(rest[1:] if rest[:1] == ["--"] else rest)
    return run(
        capability_probe_command(args.summary_out)
        if args.summary_out is not None
        else default_llvm_capabilities_command()
    )


def run_hosted_llvm_probe() -> tuple[int, dict[str, object]]:
    result = subprocess.run(
        hosted_llvm_probe_command(),
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    summary = (
        load_json(HOSTED_LLVM_CAPABILITIES_SUMMARY)
        if HOSTED_LLVM_CAPABILITIES_SUMMARY.is_file()
        else {}
    )
    return result.returncode, summary
