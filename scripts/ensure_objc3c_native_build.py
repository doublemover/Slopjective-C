#!/usr/bin/env python3
"""Shared native build helper for readiness runners and checkers."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Sequence
from objc3c_tooling.paths import display_path
from objc3c_tooling.json_io import canonical_json

ROOT = Path(__file__).resolve().parents[1]
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
SUMMARY_DEFAULT = ROOT / "tmp" / "reports" / "build-helper" / "ensure_objc3c_native_build_summary.json"
MODE_TO_EXECUTION_MODE = {
    "fast": "binaries-only",
    "contracts": "contracts-binary",
    "full": "full",
    "source-contracts": "contracts-source",
}


def safe_label(value: str) -> str:
    label = re.sub(r"[^A-Za-z0-9_.-]+", "-", value.strip().lower()).strip("-")
    return label or "native-build"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mode", choices=sorted(MODE_TO_EXECUTION_MODE), default="fast")
    parser.add_argument("--reason", default="readiness")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_DEFAULT)
    parser.add_argument("--force-reconfigure", action="store_true")
    parser.add_argument("--clean-room-root", type=Path)
    parser.add_argument("--build-dir", type=Path)
    parser.add_argument("--runtime-output-dir", type=Path)
    parser.add_argument("--library-output-dir", type=Path)
    parser.add_argument("--frontend-artifact-root", type=Path)
    parser.add_argument("--build-summary-out", type=Path)
    parser.add_argument("--parallelism", type=int, default=0)
    return parser.parse_args(argv)




def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    execution_mode = MODE_TO_EXECUTION_MODE[args.mode]
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    reason_label = safe_label(args.reason)
    log_path = args.summary_out.parent / f"ensure_objc3c_native_build_{args.mode}_{reason_label}.log"
    build_summary_path = args.build_summary_out or (
        args.summary_out.parent / f"native_build_summary_{args.mode}_{reason_label}.json"
    )

    command = [
        "pwsh",
        "-NoProfile",
        "-NonInteractive",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(BUILD_SCRIPT),
        "-ExecutionMode",
        execution_mode,
    ]
    if args.force_reconfigure:
        command.append("-ForceReconfigure")
    if args.parallelism < 0:
        print("error: parallelism must be non-negative", file=sys.stderr)
        return 2
    if args.parallelism > 0:
        command.extend(["-Parallelism", str(args.parallelism)])
    path_args = [
        ("-CleanRoomRoot", args.clean_room_root),
        ("-BuildDir", args.build_dir),
        ("-RuntimeOutputDir", args.runtime_output_dir),
        ("-LibraryOutputDir", args.library_output_dir),
        ("-FrontendArtifactRoot", args.frontend_artifact_root),
        ("-SummaryPath", build_summary_path),
    ]
    for flag, value in path_args:
        if value is not None:
            command.extend([flag, str(value)])

    completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, check=False)
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")

    combined = completed.stdout + completed.stderr
    summary = {
        "helper": "ensure_objc3c_native_build.py",
        "mode": args.mode,
        "execution_mode": execution_mode,
        "reason": args.reason,
        "force_reconfigure": args.force_reconfigure,
        "parallelism": args.parallelism,
        "ok": completed.returncode == 0,
        "returncode": completed.returncode,
        "command": command,
        "normalized_command": [display_path(Path(part)) if "\\" in part or "/" in part else part for part in command],
        "log_path": display_path(log_path),
        "clean_room_root": display_path(args.clean_room_root) if args.clean_room_root else "",
        "build_dir": display_path(args.build_dir) if args.build_dir else "",
        "runtime_output_dir": display_path(args.runtime_output_dir) if args.runtime_output_dir else "",
        "library_output_dir": display_path(args.library_output_dir) if args.library_output_dir else "",
        "frontend_artifact_root": display_path(args.frontend_artifact_root) if args.frontend_artifact_root else "",
        "build_summary_path": display_path(build_summary_path),
        "saw_cmake_configure_force_reconfigure": "cmake_configure=force-reconfigure" in combined,
        "saw_cmake_build_start": "cmake_build_start=native-binaries" in combined,
        "saw_cmake_build_skip": "cmake_build_skip=native-binaries" in combined,
        "saw_clean_room_root": "clean_room_root=" in combined,
        "saw_native_build_summary": "native_build_summary=" in combined,
        "saw_requested_parallelism": "requested_parallelism=" in combined,
        "saw_cmake_build_parallelism": "cmake_build_parallelism=" in combined,
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
