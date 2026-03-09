#!/usr/bin/env python3
"""Lane-B readiness runner for M255-B002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = (
    ["npm.cmd", "run", "build:objc3c-native"],
    ["npm.cmd", "run", "check:objc3c:m255-b001-lane-b-readiness"],
    [
        "npm.cmd",
        "run",
        "check:objc3c:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation",
    ],
    [
        "npm.cmd",
        "run",
        "test:tooling:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation",
    ],
)


def main() -> int:
    for command in COMMANDS:
        completed = subprocess.run(command, cwd=ROOT)
        if completed.returncode != 0:
            return completed.returncode
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
