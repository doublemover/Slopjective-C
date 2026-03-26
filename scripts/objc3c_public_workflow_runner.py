#!/usr/bin/env python3
"""Unified public workflow runner for M314 public package scripts."""

from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
PWsh = shutil.which("pwsh") or "pwsh"
NPX = shutil.which("npx.cmd") or shutil.which("npx") or "npx"
NPM = shutil.which("npm.cmd") or shutil.which("npm") or "npm"

BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_PS1 = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
RECOVERY_PS1 = ROOT / "scripts" / "check_objc3c_native_recovery_contract.ps1"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PROOF_PS1 = ROOT / "scripts" / "run_objc3c_native_compile_proof.ps1"
SITE_PY = ROOT / "scripts" / "build_site_index.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"


def run(command: Sequence[str]) -> int:
    return subprocess.run(list(command), cwd=ROOT, check=False).returncode


def pwsh_file(script: Path, *args: str) -> int:
    return run([PWsh, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(script), *args])


def main(argv: Sequence[str]) -> int:
    if not argv:
        print("usage: objc3c_public_workflow_runner.py <action> [args...]", file=sys.stderr)
        return 2

    action, *rest = argv

    if action == "build-default":
        return main(["build-native-binaries"])
    if action == "build-native-binaries":
        return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only")
    if action == "build-native-contracts":
        return pwsh_file(BUILD_PS1, "-ExecutionMode", "packets-binary")
    if action == "build-native-full":
        return pwsh_file(BUILD_PS1, "-ExecutionMode", "full")
    if action == "build-native-reconfigure":
        return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only", "-ForceReconfigure")
    if action == "build-spec":
        rc = run([sys.executable, str(SITE_PY)])
        if rc != 0:
            return rc
        return run([NPX, "prettier", "--write", "site/index.md"])
    if action == "compile-objc3c":
        return pwsh_file(COMPILE_PS1, *rest)
    if action == "lint-spec":
        return run([sys.executable, str(SPEC_LINT_PY)])
    if action == "test-default":
        return main(["test-smoke"])
    if action == "test-smoke":
        return main(["test-full"])
    if action == "test-ci":
        rc = run([NPM, "run", "check:task-hygiene"])
        if rc != 0:
            return rc
        rc = main(["test-full"])
        if rc != 0:
            return rc
        return run([sys.executable, "-m", "pytest", "tests/tooling", "-q"])
    if action == "test-recovery":
        return pwsh_file(RECOVERY_PS1)
    if action == "test-execution-smoke":
        return pwsh_file(SMOKE_PS1)
    if action == "test-execution-replay":
        return pwsh_file(REPLAY_PS1)
    if action == "test-full":
        rc = main(["test-recovery"])
        if rc != 0:
            return rc
        rc = run([NPM, "run", "test:objc3c:matrix"])
        if rc != 0:
            return rc
        return run([NPM, "run", "test:objc3c:negative-expectations"])
    if action == "package-runnable-toolchain":
        return pwsh_file(PACKAGE_PS1)
    if action == "proof-objc3c":
        return pwsh_file(PROOF_PS1)

    print(f"unknown action: {action}", file=sys.stderr)
    return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
