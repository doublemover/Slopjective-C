from __future__ import annotations

import argparse

from objc3c_library_cli_parity.artifacts import (
    DEFAULT_DIMENSION_MAP,
    default_dimension_map_for_emit_prefix,
    ensure_directory,
)
from objc3c_library_cli_parity.contracts import DEFAULT_ARTIFACTS
from objc3c_library_cli_parity.contracts import ParityInputs
from objc3c_library_cli_parity.contracts import SourceExecution
from objc3c_library_cli_parity.source_mode import (
    build_source_mode_artifacts,
    prepare_source_mode,
)


def load_parity_inputs(args: argparse.Namespace) -> ParityInputs:
    if args.source is not None:
        (
            library_dir,
            cli_dir,
            execution_work_key,
            execution_results,
            execution_failures,
            execution_routing,
        ) = prepare_source_mode(args)
        return ParityInputs(
            library_dir=library_dir,
            cli_dir=cli_dir,
            default_artifacts=build_source_mode_artifacts(emit_prefix=args.emit_prefix),
            default_dimension_map=default_dimension_map_for_emit_prefix(
                emit_prefix=args.emit_prefix,
                object_artifact=f"{args.emit_prefix}.obj",
            ),
            execution=SourceExecution(
                work_key=execution_work_key,
                results=execution_results,
                failures=execution_failures,
                routing=execution_routing,
            ),
        )

    if args.library_dir is None:
        raise ValueError("--library-dir is required when --source is not provided")
    if args.cli_dir is None:
        raise ValueError("--cli-dir is required when --source is not provided")
    ensure_directory(args.library_dir, label="library-dir")
    ensure_directory(args.cli_dir, label="cli-dir")
    return ParityInputs(
        library_dir=args.library_dir,
        cli_dir=args.cli_dir,
        default_artifacts=list(DEFAULT_ARTIFACTS),
        default_dimension_map=dict(DEFAULT_DIMENSION_MAP),
        execution=None,
    )


__all__ = ["load_parity_inputs"]
