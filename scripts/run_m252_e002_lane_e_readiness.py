#!/usr/bin/env python3
"""Run M252-E002 lane-E readiness checks without recursive npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m252-a003-protocol-category-property-ivar-export-graph-completion"),
    (NPM_EXECUTABLE, "run", "test:tooling:m252-a003-protocol-category-property-ivar-export-graph-completion"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m252-b004-property-ivar-export-legality-synthesis-preconditions"),
    (NPM_EXECUTABLE, "run", "test:tooling:m252-b004-property-ivar-export-legality-synthesis-preconditions"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m252-c003-metadata-debug-projection-and-replay-anchors"),
    (NPM_EXECUTABLE, "run", "test:tooling:m252-c003-metadata-debug-projection-and-replay-anchors"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads"),
    (NPM_EXECUTABLE, "run", "test:tooling:m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m252-e001-metadata-semantic-closure-gate"),
    (NPM_EXECUTABLE, "run", "test:tooling:m252-e001-metadata-semantic-closure-gate"),
    (sys.executable, "scripts/check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py",
        "-q",
    ),
)


def run_command(command: Sequence[str]) -> int:
    command_text = " ".join(command)
    print(f"[run] {command_text}")
    completed = subprocess.run(command, check=False)
    if completed.returncode != 0:
        print(f"[error] command failed with exit code {completed.returncode}: {command_text}", file=sys.stderr)
    return completed.returncode


def run_chain() -> int:
    for command in COMMAND_CHAIN:
        exit_code = run_command(command)
        if exit_code != 0:
            return exit_code

    print("[ok] M252-E002 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
