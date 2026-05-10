from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

from objc3c_library_cli_parity.artifacts import normalize_artifact_name

CLI_DESCRIPTION = "Command-line flow for Objective-C 3.0 library/CLI parity checks."


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=CLI_DESCRIPTION)
    parser.add_argument("--library-dir", type=Path)
    parser.add_argument("--cli-dir", type=Path)
    parser.add_argument("--source", type=Path)
    parser.add_argument(
        "--cli-bin",
        type=Path,
        default=None,
        help="path to objc3c-native executable when using --source mode",
    )
    parser.add_argument(
        "--c-api-bin",
        type=Path,
        default=None,
        help="path to objc3c-frontend-c-api-runner executable when using --source mode",
    )
    parser.add_argument(
        "--work-dir",
        type=Path,
        default=Path(
            "tmp/artifacts/compilation/objc3c-native/library-cli-parity/work"
        ),
        help="workspace for generated artifacts in --source mode",
    )
    parser.add_argument(
        "--work-key",
        default=None,
        help=(
            "deterministic subdirectory key under --work-dir for --source mode "
            "(default derives from source + emit-prefix)"
        ),
    )
    parser.add_argument(
        "--allow-non-tmp-work-dir",
        action="store_true",
        help="allow --source work/output directories outside repo tmp/",
    )
    parser.add_argument(
        "--allow-stale-source-mode-outputs",
        action="store_true",
        help=(
            "allow --source mode to reuse existing output paths for the current "
            "emit-prefix instead of failing on stale artifacts"
        ),
    )
    parser.add_argument(
        "--emit-prefix",
        default="module",
        help="artifact filename prefix for --source mode generation",
    )
    parser.add_argument(
        "--clang-path",
        type=Path,
        default=None,
        help="clang path forwarded to CLI and C API runner in --source mode",
    )
    parser.add_argument(
        "--llc-path",
        type=Path,
        default=None,
        help="llc path forwarded to CLI/C API runner in --source mode",
    )
    parser.add_argument(
        "--cli-ir-object-backend",
        choices=("clang", "llvm-direct"),
        default="clang",
        help="IR object backend for CLI command in --source mode",
    )
    parser.add_argument(
        "--llvm-capabilities-summary",
        type=Path,
        default=None,
        help=(
            "capability summary JSON produced by "
            "scripts/probe_objc3c_llvm_capabilities.py"
        ),
    )
    parser.add_argument(
        "--route-cli-backend-from-capabilities",
        action="store_true",
        help=(
            "derive IR object backend from --llvm-capabilities-summary "
            "(llvm-direct when supported, otherwise clang)"
        ),
    )
    parser.add_argument(
        "--objc3-max-message-args",
        type=int,
        default=None,
        help="override max message-send args forwarded to CLI/C API in --source mode",
    )
    parser.add_argument(
        "--objc3-runtime-dispatch-symbol",
        default=None,
        help="override runtime dispatch symbol forwarded to CLI/C API in --source mode",
    )
    parser.add_argument(
        "--artifacts",
        nargs="+",
        default=None,
        help="artifact filenames to compare relative to library/cli directories",
    )
    parser.add_argument(
        "--dimension-map",
        action="append",
        default=[],
        metavar="DIMENSION=ARTIFACT",
        help=(
            "override parity dimension mapping; supported dimensions are "
            "diagnostics, manifest, ir, object"
        ),
    )
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/objc3c_library_cli_parity_summary.json"),
        help="write summary JSON report to this path",
    )
    parser.add_argument(
        "--golden-summary",
        type=Path,
        default=None,
        help="path to canonical golden summary used for drift checks",
    )
    parser.add_argument(
        "--check-golden",
        action="store_true",
        help="fail when computed summary does not exactly match --golden-summary",
    )
    parser.add_argument(
        "--write-golden",
        action="store_true",
        help="write computed summary to --golden-summary",
    )
    return parser


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    return build_parser().parse_args(argv)


def validate_golden_options(args: argparse.Namespace) -> None:
    if args.check_golden and args.write_golden:
        raise ValueError("--check-golden and --write-golden cannot be used together")
    if (args.check_golden or args.write_golden) and args.golden_summary is None:
        raise ValueError(
            "--golden-summary is required when using --check-golden/--write-golden"
        )


def normalize_cli_config(args: argparse.Namespace) -> None:
    args.emit_prefix = normalize_artifact_name(
        args.emit_prefix,
        context="--emit-prefix",
    )


__all__ = [
    "build_parser",
    "normalize_cli_config",
    "parse_args",
    "validate_golden_options",
]
