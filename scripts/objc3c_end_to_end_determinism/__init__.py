"""End-to-end determinism checker package facade."""

from __future__ import annotations

from scripts.objc3c_end_to_end_determinism.artifacts import collect_artifacts
from scripts.objc3c_end_to_end_determinism.cli import build_parser, check_determinism, main
from scripts.objc3c_end_to_end_determinism.commands import (
    expand_command_tokens,
    normalize_command_tokens,
    parse_key_value,
    parse_variant_env,
    run_once,
)
from scripts.objc3c_end_to_end_determinism.comparison import compare_runs
from scripts.objc3c_end_to_end_determinism.constants import (
    DEFAULT_REPLAY_ROOT,
    MAX_STDIO_PREVIEW_CHARS,
    MODE,
    ROOT,
)
from scripts.objc3c_end_to_end_determinism.errors import DeterminismContractError
from scripts.objc3c_end_to_end_determinism.hashing import sha256_bytes
from scripts.objc3c_end_to_end_determinism.reports import (
    build_summary_payload,
    emit_failure_report,
    emit_success_report,
    write_summary_payload,
)

__all__ = [
    "DEFAULT_REPLAY_ROOT",
    "MAX_STDIO_PREVIEW_CHARS",
    "MODE",
    "ROOT",
    "DeterminismContractError",
    "build_parser",
    "build_summary_payload",
    "check_determinism",
    "collect_artifacts",
    "compare_runs",
    "emit_failure_report",
    "emit_success_report",
    "expand_command_tokens",
    "main",
    "normalize_command_tokens",
    "parse_key_value",
    "parse_variant_env",
    "run_once",
    "sha256_bytes",
    "write_summary_payload",
]
