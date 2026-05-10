from __future__ import annotations

import argparse
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import canonical_json
from objc3c_tooling.paths import display_path

from objc3c_library_cli_parity.artifacts import (
    ensure_file,
    ensure_under_tmp,
    normalize_work_key,
    sha256_text,
)
from objc3c_library_cli_parity.capabilities import read_capability_summary
from objc3c_library_cli_parity.subprocesses import (
    CommandResult,
    format_command,
    run_command,
)

def build_source_mode_artifacts(*, emit_prefix: str) -> list[str]:
    return [
        f"{emit_prefix}.diagnostics.json",
        f"{emit_prefix}.manifest.json",
        f"{emit_prefix}.ll",
        f"{emit_prefix}.obj",
    ]


def default_source_mode_work_key(args: argparse.Namespace) -> str:
    fingerprint: dict[str, Any] = {
        "source": display_path(args.source),
        "emit_prefix": args.emit_prefix,
        "cli_bin": display_path(args.cli_bin),
        "c_api_bin": display_path(args.c_api_bin),
        "cli_ir_object_backend": args.cli_ir_object_backend,
        "clang_path": display_path(args.clang_path) if args.clang_path is not None else None,
        "llc_path": display_path(args.llc_path) if args.llc_path is not None else None,
        "llvm_capabilities_summary": (
            display_path(args.llvm_capabilities_summary)
            if args.llvm_capabilities_summary is not None
            else None
        ),
        "route_cli_backend_from_capabilities": args.route_cli_backend_from_capabilities,
        "objc3_max_message_args": args.objc3_max_message_args,
        "objc3_runtime_dispatch_symbol": args.objc3_runtime_dispatch_symbol,
    }
    return sha256_text(canonical_json(fingerprint))[:16]


def assert_no_stale_source_mode_outputs(
    *,
    directory: Path,
    emit_prefix: str,
    label: str,
) -> None:
    stale_paths: list[str] = []
    expected_artifacts = list(build_source_mode_artifacts(emit_prefix=emit_prefix))
    expected_artifacts.extend(
        (
            f"{emit_prefix}.diagnostics.txt",
            f"{emit_prefix}.c_api_summary.json",
        )
    )
    for artifact_name in expected_artifacts:
        artifact_path = directory / artifact_name
        proxy_path = directory / f"{artifact_name}.sha256"
        if artifact_path.exists():
            stale_paths.append(display_path(artifact_path))
        if proxy_path.exists():
            stale_paths.append(display_path(proxy_path))
    if stale_paths:
        stale_display = ", ".join(sorted(set(stale_paths)))
        raise ValueError(
            f"{label} contains stale generated artifacts; choose a unique --work-key "
            f"or clear outputs before replay: {stale_display}"
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

    work_key = args.work_key
    if work_key is None:
        work_key = default_source_mode_work_key(args)
    else:
        work_key = normalize_work_key(work_key)
    work_root = work_dir / work_key

    library_dir = args.library_dir if args.library_dir is not None else work_root / "library"
    cli_dir = args.cli_dir if args.cli_dir is not None else work_root / "cli"
    if not args.allow_non_tmp_work_dir:
        ensure_under_tmp(library_dir, label="library-dir")
        ensure_under_tmp(cli_dir, label="cli-dir")
    library_dir.mkdir(parents=True, exist_ok=True)
    cli_dir.mkdir(parents=True, exist_ok=True)
    if not args.allow_stale_source_mode_outputs:
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

    effective_clang_path = args.clang_path
    effective_llc_path = args.llc_path
    effective_backend = args.cli_ir_object_backend
    routing: dict[str, Any] = {
        "requested_cli_ir_object_backend": args.cli_ir_object_backend,
        "effective_ir_object_backend": effective_backend,
        "routed_from_capabilities": args.route_cli_backend_from_capabilities,
        "allow_stale_source_mode_outputs": args.allow_stale_source_mode_outputs,
    }
    capability_failures: list[str] = []
    if args.llvm_capabilities_summary is not None:
        try:
            capability_summary = read_capability_summary(args.llvm_capabilities_summary)
        except ValueError as exc:
            capability_failures.append(
                f"capability routing fail-closed: {exc}"
            )
            return library_dir, cli_dir, work_key, [], capability_failures, routing
        routing["llvm_capabilities_summary"] = capability_summary.summary_path
        routing["llvm_capabilities"] = {
            "clang_found": capability_summary.clang_found,
            "llc_found": capability_summary.llc_found,
            "llc_supports_filetype_obj": capability_summary.llc_supports_filetype_obj,
            "parity_ready": capability_summary.parity_ready,
            "blockers": list(capability_summary.blockers),
        }
        if not capability_summary.parity_ready:
            blockers = ", ".join(capability_summary.blockers) if capability_summary.blockers else "unspecified"
            capability_failures.append(
                "capability routing fail-closed: sema/type-system parity capability unavailable: "
                f"{blockers}"
            )
        if args.route_cli_backend_from_capabilities:
            effective_backend = (
                "llvm-direct" if capability_summary.llc_supports_filetype_obj else "clang"
            )
        if effective_backend == "clang" and not capability_summary.clang_found:
            capability_failures.append(
                "capability routing fail-closed: clang backend selected but capability summary reports clang unavailable"
            )
        if effective_backend == "llvm-direct" and (
            not capability_summary.llc_found or not capability_summary.llc_supports_filetype_obj
        ):
            capability_failures.append(
                "capability routing fail-closed: llvm-direct backend selected but llc --filetype=obj capability is unavailable"
            )
        if effective_clang_path is None:
            effective_clang_path = Path(capability_summary.clang_path)
        if effective_llc_path is None:
            effective_llc_path = Path(capability_summary.llc_path)

    if args.route_cli_backend_from_capabilities and args.llvm_capabilities_summary is None:
        capability_failures.append(
            "capability routing fail-closed: --route-cli-backend-from-capabilities requires --llvm-capabilities-summary"
        )

    routing["effective_ir_object_backend"] = effective_backend
    if effective_clang_path is not None:
        routing["effective_clang_path"] = display_path(effective_clang_path)
    if effective_llc_path is not None:
        routing["effective_llc_path"] = display_path(effective_llc_path)

    if capability_failures:
        return library_dir, cli_dir, work_key, [], capability_failures, routing

    cli_command: list[str] = [
        str(args.cli_bin),
        str(args.source),
        "--out-dir",
        str(cli_dir),
        "--emit-prefix",
        args.emit_prefix,
        "--objc3-ir-object-backend",
        effective_backend,
    ]
    if effective_clang_path is not None:
        cli_command.extend(["--clang", str(effective_clang_path)])
    if effective_llc_path is not None:
        cli_command.extend(["--llc", str(effective_llc_path)])
    if args.objc3_max_message_args is not None:
        cli_command.extend(["--objc3-max-message-args", str(args.objc3_max_message_args)])
    if args.objc3_runtime_dispatch_symbol:
        cli_command.extend(["--objc3-runtime-dispatch-symbol", args.objc3_runtime_dispatch_symbol])

    c_api_command: list[str] = [
        str(args.c_api_bin),
        str(args.source),
        "--out-dir",
        str(library_dir),
        "--emit-prefix",
        args.emit_prefix,
        "--objc3-ir-object-backend",
        effective_backend,
    ]
    if effective_clang_path is not None:
        c_api_command.extend(["--clang", str(effective_clang_path)])
    if effective_llc_path is not None:
        c_api_command.extend(["--llc", str(effective_llc_path)])
    if args.objc3_max_message_args is not None:
        c_api_command.extend(["--objc3-max-message-args", str(args.objc3_max_message_args)])
    if args.objc3_runtime_dispatch_symbol:
        c_api_command.extend(["--objc3-runtime-dispatch-symbol", args.objc3_runtime_dispatch_symbol])

    results = [
        run_command("cli", cli_command),
        run_command("c-api", c_api_command),
    ]
    failures = [
        f"{result.role} command failed with exit {result.exit_code}: {format_command(result.command)}"
        for result in results
        if result.exit_code != 0
    ]
    return library_dir, cli_dir, work_key, results, failures, routing


