from __future__ import annotations

import argparse
from pathlib import Path
from typing import Any

from objc3c_library_cli_parity.artifacts import ensure_file
from objc3c_library_cli_parity.artifacts import ensure_under_tmp
from objc3c_library_cli_parity.artifacts import normalize_work_key
from objc3c_library_cli_parity.source_mode.artifacts import (
    assert_no_stale_source_mode_outputs,
)
from objc3c_library_cli_parity.source_mode.commands import run_source_mode_commands
from objc3c_library_cli_parity.source_mode.routing import resolve_source_mode_routing
from objc3c_library_cli_parity.source_mode.work_key import default_source_mode_work_key
from objc3c_library_cli_parity.subprocesses import CommandResult


def _prepare_source_mode_directories(
    args: argparse.Namespace,
    *,
    work_key: str,
) -> tuple[Path, Path]:
    work_root = args.work_dir / work_key
    library_dir = (
        args.library_dir if args.library_dir is not None else work_root / "library"
    )
    cli_dir = args.cli_dir if args.cli_dir is not None else work_root / "cli"
    if not args.allow_non_tmp_work_dir:
        ensure_under_tmp(library_dir, label="library-dir")
        ensure_under_tmp(cli_dir, label="cli-dir")
    library_dir.mkdir(parents=True, exist_ok=True)
    cli_dir.mkdir(parents=True, exist_ok=True)
    return library_dir, cli_dir


def _assert_source_mode_outputs_are_fresh(
    args: argparse.Namespace,
    *,
    library_dir: Path,
    cli_dir: Path,
) -> None:
    if args.allow_stale_source_mode_outputs:
        return
    assert_no_stale_source_mode_outputs(
        directory=library_dir,
        emit_prefix=args.emit_prefix,
        label="library-dir",
    )
    assert_no_stale_source_mode_outputs(
        directory=cli_dir,
        emit_prefix=args.emit_prefix,
        label="cli-dir",
    )


def prepare_source_mode(
    args: argparse.Namespace,
) -> tuple[Path, Path, str, list[CommandResult], list[str], dict[str, Any]]:
    if args.cli_bin is None:
        raise ValueError("--cli-bin is required when using --source")
    if args.c_api_bin is None:
        raise ValueError("--c-api-bin is required when using --source")
    ensure_file(args.source, label="source")
    ensure_file(args.cli_bin, label="cli-bin")
    ensure_file(args.c_api_bin, label="c-api-bin")

    work_dir = args.work_dir
    if not args.allow_non_tmp_work_dir:
        ensure_under_tmp(work_dir, label="work-dir")
    work_dir.mkdir(parents=True, exist_ok=True)

    work_key = (
        default_source_mode_work_key(args)
        if args.work_key is None
        else normalize_work_key(args.work_key)
    )
    library_dir, cli_dir = _prepare_source_mode_directories(args, work_key=work_key)
    _assert_source_mode_outputs_are_fresh(args, library_dir=library_dir, cli_dir=cli_dir)

    routing = resolve_source_mode_routing(args)
    if routing.failures:
        return library_dir, cli_dir, work_key, [], routing.failures, routing.details

    results, failures = run_source_mode_commands(
        args,
        library_dir=library_dir,
        cli_dir=cli_dir,
        effective_backend=routing.effective_backend,
        effective_clang_path=routing.effective_clang_path,
        effective_llc_path=routing.effective_llc_path,
    )
    return library_dir, cli_dir, work_key, results, failures, routing.details
