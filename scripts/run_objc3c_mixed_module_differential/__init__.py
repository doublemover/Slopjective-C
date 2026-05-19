"""Implementation modules for the mixed-module differential runner."""

from __future__ import annotations

from .cli import build_arg_parser, parse_args
from .commands import CASE_RUNNERS, SURFACE_BUILDERS, CaseCommand, run_no_clang, run_with_clang
from .config import MANIFEST_PATH, ROOT, SUMMARY_CONTRACT_ID, SUMMARY_PATH, RunnerConfig
from .fixtures import load_manifest, prepare_run_root
from .models import CaseRunner, CaseSummary, ClangCase, FixtureGroup, NoClangCase, SurfaceBuilder
from .native import NativeToolResolver
from .orchestration import main, run_cases, run_mixed_module_differential
from .reporting import (
    build_summary_payload,
    build_surfaces,
    emit_result,
    render_console_summary,
    write_json,
)

__all__ = [
    "CASE_RUNNERS",
    "MANIFEST_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "SUMMARY_PATH",
    "SURFACE_BUILDERS",
    "CaseCommand",
    "CaseRunner",
    "CaseSummary",
    "ClangCase",
    "FixtureGroup",
    "NativeToolResolver",
    "NoClangCase",
    "RunnerConfig",
    "SurfaceBuilder",
    "build_arg_parser",
    "build_summary_payload",
    "build_surfaces",
    "emit_result",
    "load_manifest",
    "main",
    "parse_args",
    "prepare_run_root",
    "render_console_summary",
    "run_cases",
    "run_mixed_module_differential",
    "run_no_clang",
    "run_with_clang",
    "write_json",
]
