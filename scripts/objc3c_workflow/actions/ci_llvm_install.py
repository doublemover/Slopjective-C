"""Explicit host routing for the existing hosted LLVM installers."""

from __future__ import annotations

import argparse
import sys

from ..commands import pwsh_file, run
from ..environment import SCRIPT_ROOT


def action_materialize_ci_llvm(rest: list[str]) -> int:
    parser = argparse.ArgumentParser(prog="npm run objc3c -- materialize-ci-llvm")
    parser.add_argument(
        "--platform", required=True,
        choices=("windows-x64", "linux-x64", "darwin-arm64"),
    )
    args = parser.parse_args(rest[1:] if rest[:1] == ["--"] else rest)
    hosts = {"windows-x64": "win32", "linux-x64": "linux", "darwin-arm64": "darwin"}
    if sys.platform != hosts[args.platform]:
        parser.error(f"{args.platform} requires a {hosts[args.platform]} host")
    if args.platform == "windows-x64":
        return pwsh_file(SCRIPT_ROOT / "install_objc3c_ci_llvm.ps1")
    suffix = "linux" if args.platform == "linux-x64" else "darwin"
    return run(["bash", str(SCRIPT_ROOT / f"install_objc3c_ci_llvm_{suffix}.sh")])
