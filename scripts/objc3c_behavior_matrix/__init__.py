"""Public facade for the objc3c native behavior fixture matrix."""

from __future__ import annotations

import sys
from pathlib import Path

SCRIPTS_DIR = Path(__file__).resolve().parent.parent
if str(SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_DIR))

from objc3c_tooling.behavior_fixtures import BehaviorFixture, REQUIRED_TREE, load_behavior_fixtures
from objc3c_tooling.paths import ROOT, repo_rel
from objc3c_tooling.subprocesses import CommandExecution

from .case_execution import execute_fixture
from .cli import parse_args
from .config import BUILD_SCRIPT, CONTRACT_ID, NATIVE_EXE, REPORT_PATH, RUNTIME_LIB
from .errors import BehaviorMatrixFailure
from .expectations import assert_tokens, missing_tokens
from .fixture_loading import select_fixtures
from .native_execution import (
    canonical_link_text,
    clang_command,
    compile_fixture,
    compile_output_text,
    ensure_native_binaries,
    link_fixture,
    object_path,
    run_executable,
)
from .reporting import build_summary_payload, render_report, write_report
from .runner import main, resolve_report_path, run_matrix

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
