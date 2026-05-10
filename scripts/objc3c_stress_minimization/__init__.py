"""Implementation modules for the objc3c stress-minimization runner."""

from __future__ import annotations

from .cli import build_arg_parser, main, parse_args
from .execution import compile_source
from .fixtures import load_manifest, validate_artifact_surface
from .minimization import reduce_source
from .models import MinCase
from .paths import ARTIFACT_SURFACE_PATH, COMPILER, MANIFEST_PATH, ROOT, SUMMARY_CONTRACT_ID, SUMMARY_PATH
from .reporting import (
    build_failure_summary,
    build_invocation_payload,
    build_reduced_summary,
    build_reducer_plan,
    build_summary_payload,
    emit_result,
    json_text,
    render_console_summary,
    write_json,
)
from .runner import materialize_case, run_stress_minimization

__all__ = [
    "ARTIFACT_SURFACE_PATH",
    "COMPILER",
    "MANIFEST_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "SUMMARY_PATH",
    "MinCase",
    "build_arg_parser",
    "build_failure_summary",
    "build_invocation_payload",
    "build_reduced_summary",
    "build_reducer_plan",
    "build_summary_payload",
    "compile_source",
    "emit_result",
    "json_text",
    "load_manifest",
    "main",
    "materialize_case",
    "parse_args",
    "reduce_source",
    "render_console_summary",
    "run_stress_minimization",
    "validate_artifact_surface",
    "write_json",
]
