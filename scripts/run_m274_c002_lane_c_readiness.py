#!/usr/bin/env python3
"""Lane-C readiness runner for M274-C002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m274_c002_foreign_call_and_lifetime_lowering_core_feature_implementation.py"
PYTEST = ROOT / "tests" / "tooling" / "test_check_m274_c002_foreign_call_and_lifetime_lowering_core_feature_implementation.py"


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def main() -> int:
    build_docs = run([sys.executable, str(ROOT / "scripts" / "build_objc3c_native_docs.py")])
    if build_docs.returncode != 0:
        print(build_docs.stdout, end="")
        print(build_docs.stderr, end="", file=sys.stderr)
        return build_docs.returncode

    checker = run([sys.executable, str(CHECKER)])
    if checker.returncode != 0:
        print(checker.stdout, end="")
        print(checker.stderr, end="", file=sys.stderr)
        return checker.returncode

    pytest_run = run([sys.executable, "-m", "pytest", str(PYTEST), "-q"])
    if pytest_run.returncode != 0:
        print(pytest_run.stdout, end="")
        print(pytest_run.stderr, end="", file=sys.stderr)
        return pytest_run.returncode

    print(build_docs.stdout, end="")
    print(checker.stdout, end="")
    print(pytest_run.stdout, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())