#!/usr/bin/env python3
"""Run the behavior-first native fixture matrix from tests/native."""

from __future__ import annotations

import sys
from pathlib import Path

SCRIPTS_DIR = Path(__file__).resolve().parent
if str(SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_DIR))

from objc3c_behavior_matrix import (
    BUILD_SCRIPT,
    CONTRACT_ID,
    NATIVE_EXE,
    REQUIRED_TREE,
    REPORT_PATH,
    ROOT,
    RUNTIME_LIB,
    BehaviorFixture,
    BehaviorMatrixFailure,
    CommandExecution,
    assert_tokens,
    build_summary_payload,
    canonical_link_text,
    clang_command,
    compile_fixture,
    compile_output_text,
    ensure_native_binaries,
    execute_fixture,
    link_fixture,
    load_behavior_fixtures,
    main,
    missing_tokens,
    object_path,
    parse_args,
    render_report,
    repo_rel,
    resolve_report_path,
    run_executable,
    run_matrix,
    select_fixtures,
    write_report,
)

__all__ = [
    "BUILD_SCRIPT",
    "CONTRACT_ID",
    "NATIVE_EXE",
    "REQUIRED_TREE",
    "REPORT_PATH",
    "ROOT",
    "RUNTIME_LIB",
    "BehaviorFixture",
    "BehaviorMatrixFailure",
    "CommandExecution",
    "assert_tokens",
    "build_summary_payload",
    "canonical_link_text",
    "clang_command",
    "compile_fixture",
    "compile_output_text",
    "ensure_native_binaries",
    "execute_fixture",
    "link_fixture",
    "load_behavior_fixtures",
    "main",
    "missing_tokens",
    "object_path",
    "parse_args",
    "render_report",
    "repo_rel",
    "resolve_report_path",
    "run_executable",
    "run_matrix",
    "select_fixtures",
    "write_report",
]


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
