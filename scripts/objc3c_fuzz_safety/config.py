"""Configuration and input validation for the objc3c fuzz-safety runner."""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
from pathlib import Path

from objc3c_tooling.artifact_identity import current_host_artifact_identity
from objc3c_tooling.paths import display_path

ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_IDENTITY = current_host_artifact_identity()
DEFAULT_COMPILER = ROOT / ARTIFACT_IDENTITY.native_executable_relative_path
DEFAULT_OUT_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "fuzz-safety"
DEFAULT_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "parser_sema_fuzz_manifest.json"
MODE = "objc3c-fuzz-safety-v1"
SCHEMA_VERSION = "objc3c-fuzz-safety-report.v1"

EXIT_OK = 0
EXIT_GUARD_FAIL = 1
EXIT_INPUT_ERROR = 2


class InputError(ValueError):
    """Raised when CLI input cannot be normalized safely."""


def parse_generated_at_utc(value: str | None) -> str:
    if value is None:
        return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    candidate = value.strip()
    try:
        datetime.strptime(candidate, "%Y-%m-%dT%H:%M:%SZ")
    except ValueError as exc:
        raise InputError(
            "--generated-at-utc must be UTC format YYYY-MM-DDTHH:MM:SSZ"
        ) from exc
    return candidate


def validate_inputs(args: argparse.Namespace) -> None:
    if args.timeout_sec <= 0:
        raise InputError("--timeout-sec must be > 0")
    if args.max_cases is not None and args.max_cases <= 0:
        raise InputError("--max-cases must be > 0")

    compiler = args.compiler.resolve()
    if not compiler.exists() or not compiler.is_file():
        raise InputError(f"compiler not found: {display_path(compiler)}")
    manifest = args.manifest.resolve()
    if not manifest.exists() or not manifest.is_file():
        raise InputError(f"manifest not found: {display_path(manifest)}")

    if args.python_launcher is not None:
        launcher = args.python_launcher.resolve()
        if not launcher.exists() or not launcher.is_file():
            raise InputError(f"python launcher not found: {display_path(launcher)}")
