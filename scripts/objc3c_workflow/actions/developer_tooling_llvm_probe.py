"""LLVM capability probe workflow helpers."""

from __future__ import annotations

import subprocess
import sys

from objc3c_tooling.json_io import load_json_object as load_json

from ..commands import run
from ..environment import ROOT
from .developer_tooling_paths import (
    HOSTED_LLVM_CAPABILITIES_SUMMARY,
    LLVM_CAPABILITIES_PROBE_PY,
)


def action_check_llvm_capabilities(_: list[str]) -> int:
    return run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            "tmp/artifacts/objc3c-native/llvm_capabilities/summary.json",
        ]
    )


def run_hosted_llvm_probe() -> tuple[int, dict[str, object]]:
    result = subprocess.run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            str(HOSTED_LLVM_CAPABILITIES_SUMMARY.relative_to(ROOT).as_posix()),
        ],
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
