#!/usr/bin/env python3
"""Run M255-A002 lane-A readiness checks without recursive npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M255-A002"
CHECK_SCRIPT = (
    "check:objc3c:m255-a002-instance-class-super-direct-and-dynamic-"
    "dispatch-site-modeling-core-feature-implementation"
)
NATIVE_BINARIES = (
    ROOT / "artifacts" / "bin" / "objc3c-native.exe",
    ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe",
)
SOURCE_INPUTS = (
    ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h",
    ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp",
    ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h",
    ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
)
COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py",
        "-q",
    ),
)


def binaries_stale() -> bool:
    if any(not path.exists() for path in NATIVE_BINARIES):
        return True
    newest_binary = min(path.stat().st_mtime for path in NATIVE_BINARIES)
    return any(path.stat().st_mtime > newest_binary for path in SOURCE_INPUTS)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (A002 builds on the frozen A001 dispatch taxonomy)"
    )
    print(f"[info] canonical checker script: {CHECK_SCRIPT}")
    if binaries_stale():
        build_command = (NPM_EXECUTABLE, "run", "build:objc3c-native")
        build_text = " ".join(build_command)
        print(f"[run] {build_text}")
        completed = subprocess.run(build_command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {build_text}",
                file=sys.stderr,
            )
            return completed.returncode
    else:
        print("[info] skipping build: objc3c-native binaries are already fresh for M255-A002 inputs")
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M255-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
