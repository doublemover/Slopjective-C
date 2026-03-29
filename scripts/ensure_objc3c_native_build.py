#!/usr/bin/env python3
"""Shared native build helper for readiness runners and checkers."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
SUMMARY_DEFAULT = ROOT / "tmp" / "reports" / "build-helper" / "ensure_objc3c_native_build_summary.json"
MODE_TO_EXECUTION_MODE = {
    "fast": "binaries-only",
    "contracts": "contracts-binary",
    "full": "full",
}


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mode", choices=sorted(MODE_TO_EXECUTION_MODE), default="fast")
    parser.add_argument("--reason", default="readiness")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_DEFAULT)
    parser.add_argument("--force-reconfigure", action="store_true")
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    execution_mode = MODE_TO_EXECUTION_MODE[args.mode]
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    log_path = args.summary_out.parent / f"ensure_objc3c_native_build_{args.mode}_{int(time.time())}.log"

    command = [
        "pwsh",
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(BUILD_SCRIPT),
        "-ExecutionMode",
        execution_mode,
    ]
    if args.force_reconfigure:
        command.append("-ForceReconfigure")

    completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, check=False)
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")

    combined = completed.stdout + completed.stderr
    summary = {
        "helper": "ensure_objc3c_native_build.py",
        "mode": args.mode,
        "execution_mode": execution_mode,
        "reason": args.reason,
        "force_reconfigure": args.force_reconfigure,
        "ok": completed.returncode == 0,
        "returncode": completed.returncode,
        "command": command,
        "log_path": display_path(log_path),
        "saw_cmake_configure_force_reconfigure": "cmake_configure=force-reconfigure" in combined,
        "saw_cmake_build_start": "cmake_build_start=native-binaries" in combined,
        "saw_cmake_build_skip": "cmake_build_skip=native-binaries" in combined,
        "saw_contract_mode": f"artifact_generation_mode={execution_mode}" in combined or f"artifact_generation_mode={args.mode}" in combined,
    }
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if completed.returncode != 0:
        print(completed.stdout, end="")
        print(completed.stderr, end="", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return completed.returncode

    print(f"[ok] ensured native build ({args.mode})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
