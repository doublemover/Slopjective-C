from __future__ import annotations

import argparse
from pathlib import Path

from objc3c_library_cli_parity.subprocesses import CommandResult
from objc3c_library_cli_parity.subprocesses import format_command
from objc3c_library_cli_parity.subprocesses import run_command


def build_source_mode_command(
    *,
    binary: Path,
    source: Path,
    output_dir: Path,
    emit_prefix: str,
    backend: str,
    clang_path: Path | None,
    llc_path: Path | None,
    max_message_args: int | None,
    runtime_dispatch_symbol: str | None,
) -> list[str]:
    command: list[str] = [
        str(binary),
        str(source),
        "--out-dir",
        str(output_dir),
        "--emit-prefix",
        emit_prefix,
        "--objc3-ir-object-backend",
        backend,
    ]
    if clang_path is not None:
        command.extend(["--clang", str(clang_path)])
    if llc_path is not None:
        command.extend(["--llc", str(llc_path)])
    if max_message_args is not None:
        command.extend(["--objc3-max-message-args", str(max_message_args)])
    if runtime_dispatch_symbol:
        command.extend(["--objc3-runtime-dispatch-symbol", runtime_dispatch_symbol])
    return command


def build_cli_source_mode_command(
    args: argparse.Namespace,
    *,
    cli_dir: Path,
    effective_backend: str,
    effective_clang_path: Path | None,
    effective_llc_path: Path | None,
) -> list[str]:
    return build_source_mode_command(
        binary=args.cli_bin,
        source=args.source,
        output_dir=cli_dir,
        emit_prefix=args.emit_prefix,
        backend=effective_backend,
        clang_path=effective_clang_path,
        llc_path=effective_llc_path,
        max_message_args=args.objc3_max_message_args,
        runtime_dispatch_symbol=args.objc3_runtime_dispatch_symbol,
    )


def build_c_api_source_mode_command(
    args: argparse.Namespace,
    *,
    library_dir: Path,
    effective_backend: str,
    effective_clang_path: Path | None,
    effective_llc_path: Path | None,
) -> list[str]:
    return build_source_mode_command(
        binary=args.c_api_bin,
        source=args.source,
        output_dir=library_dir,
        emit_prefix=args.emit_prefix,
        backend=effective_backend,
        clang_path=effective_clang_path,
        llc_path=effective_llc_path,
        max_message_args=args.objc3_max_message_args,
        runtime_dispatch_symbol=args.objc3_runtime_dispatch_symbol,
    )


def run_source_mode_commands(
    args: argparse.Namespace,
    *,
    library_dir: Path,
    cli_dir: Path,
    effective_backend: str,
    effective_clang_path: Path | None,
    effective_llc_path: Path | None,
) -> tuple[list[CommandResult], list[str]]:
    cli_command = build_cli_source_mode_command(
        args,
        cli_dir=cli_dir,
        effective_backend=effective_backend,
        effective_clang_path=effective_clang_path,
        effective_llc_path=effective_llc_path,
    )
    c_api_command = build_c_api_source_mode_command(
        args,
        library_dir=library_dir,
        effective_backend=effective_backend,
        effective_clang_path=effective_clang_path,
        effective_llc_path=effective_llc_path,
    )
    results = [
        run_command("cli", cli_command),
        run_command("c-api", c_api_command),
    ]
    failures = [
        f"{result.role} command failed with exit {result.exit_code}: {format_command(result.command)}"
        for result in results
        if result.exit_code != 0
    ]
    return results, failures
