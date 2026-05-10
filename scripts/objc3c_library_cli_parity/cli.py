"""Command-line flow for Objective-C 3.0 library/CLI parity checks."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.json_io import canonical_json, write_json_file as write_json
from objc3c_tooling.paths import display_path

from objc3c_library_cli_parity.artifacts import (
    DEFAULT_DIMENSION_MAP,
    build_dimension_results,
    default_dimension_map_for_emit_prefix,
    ensure_directory,
    normalize_artifact_name,
    normalize_artifacts,
    parse_dimension_map,
    resolve_artifact_digest,
    sha256_text,
)
from objc3c_library_cli_parity.fixtures import (
    synthetic_fixture_contract_payload,
    synthetic_fixture_summary_envelope,
    validate_synthetic_fixture_contract,
)
from objc3c_library_cli_parity.source_mode import (
    build_source_mode_artifacts,
    prepare_source_mode,
)
from objc3c_library_cli_parity.subprocesses import CommandResult

MODE = "objc3c-library-cli-parity-v2"
DEFAULT_ARTIFACTS = (
    "module.diagnostics.json",
    "module.manifest.json",
    "module.ll",
    "module.o",
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
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
        default=Path("tmp/artifacts/compilation/objc3c-native/library-cli-parity/work"),
        help="workspace for generated artifacts in --source mode",
    )
    parser.add_argument(
        "--work-key",
        default=None,
        help="deterministic subdirectory key under --work-dir for --source mode (default derives from source + emit-prefix)",
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
        help="capability summary JSON produced by scripts/probe_objc3c_llvm_capabilities.py",
    )
    parser.add_argument(
        "--route-cli-backend-from-capabilities",
        action="store_true",
        help="derive IR object backend from --llvm-capabilities-summary (llvm-direct when supported, otherwise clang)",
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
    return parser.parse_args(argv)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    if args.check_golden and args.write_golden:
        raise ValueError("--check-golden and --write-golden cannot be used together")
    if (args.check_golden or args.write_golden) and args.golden_summary is None:
        raise ValueError("--golden-summary is required when using --check-golden/--write-golden")
    args.emit_prefix = normalize_artifact_name(
        args.emit_prefix,
        context="--emit-prefix",
    )

    execution_results: list[CommandResult] = []
    execution_failures: list[str] = []
    execution_work_key: str | None = None
    execution_routing: dict[str, Any] | None = None
    if args.source is not None:
        (
            library_dir,
            cli_dir,
            execution_work_key,
            execution_results,
            execution_failures,
            execution_routing,
        ) = prepare_source_mode(args)
        default_dimension_map = default_dimension_map_for_emit_prefix(
            emit_prefix=args.emit_prefix,
            object_artifact=f"{args.emit_prefix}.obj",
        )
        default_artifacts = build_source_mode_artifacts(emit_prefix=args.emit_prefix)
    else:
        if args.library_dir is None:
            raise ValueError("--library-dir is required when --source is not provided")
        if args.cli_dir is None:
            raise ValueError("--cli-dir is required when --source is not provided")
        library_dir = args.library_dir
        cli_dir = args.cli_dir
        ensure_directory(library_dir, label="library-dir")
        ensure_directory(cli_dir, label="cli-dir")
        default_dimension_map = dict(DEFAULT_DIMENSION_MAP)
        default_artifacts = list(DEFAULT_ARTIFACTS)

    artifacts = normalize_artifacts(args.artifacts or default_artifacts)
    dimension_map = parse_dimension_map(
        args.dimension_map,
        default_mapping=default_dimension_map,
    )

    failures: list[str] = list(execution_failures)
    comparisons: list[dict[str, Any]] = []
    artifact_to_dimension = {
        artifact: dimension
        for dimension, artifact in dimension_map.items()
    }
    for artifact_name in artifacts:
        try:
            library_digest = resolve_artifact_digest(
                base_dir=library_dir,
                artifact_name=artifact_name,
            )
        except ValueError as exc:
            failures.append(f"library {artifact_name}: {exc}")
            continue
        try:
            cli_digest = resolve_artifact_digest(
                base_dir=cli_dir,
                artifact_name=artifact_name,
            )
        except ValueError as exc:
            failures.append(f"cli {artifact_name}: {exc}")
            continue

        if library_digest.source_kind != cli_digest.source_kind:
            failures.append(
                f"source-kind mismatch for {artifact_name}: "
                f"library={library_digest.source_kind} cli={cli_digest.source_kind}"
            )
            continue

        library_sha = library_digest.sha256
        cli_sha = cli_digest.sha256
        matches = library_sha == cli_sha
        if not matches:
            failures.append(
                f"digest mismatch for {artifact_name}: "
                f"library={library_sha[:16]} cli={cli_sha[:16]}"
            )

        dimension = artifact_to_dimension.get(artifact_name)
        comparisons.append(
            {
                "artifact": artifact_name,
                "dimension": dimension if dimension is not None else "extra",
                "source_kind": library_digest.source_kind,
                "library_source": library_digest.source_path,
                "cli_source": cli_digest.source_path,
                "library_sha256": library_sha,
                "cli_sha256": cli_sha,
                "matches": matches,
            }
        )

    dimension_results = build_dimension_results(
        artifacts=artifacts,
        dimension_map=dimension_map,
    )

    summary: dict[str, Any] = {
        "mode": MODE,
        "library_dir": display_path(library_dir),
        "cli_dir": display_path(cli_dir),
        "artifacts": artifacts,
        "dimensions": dimension_results,
        "comparisons": comparisons,
        "failures": failures,
        "ok": not failures,
    }

    synthetic_contract_applied, synthetic_failures, synthetic_details = (
        validate_synthetic_fixture_contract(
            library_dir=library_dir,
            cli_dir=cli_dir,
        )
    )
    if synthetic_contract_applied:
        summary["artifact_authenticity"] = synthetic_fixture_summary_envelope()
        summary["synthetic_fixture_contract"] = synthetic_fixture_contract_payload()
        summary["authenticity_checks"] = {
            **(synthetic_details or {}),
            "failure_count": len(synthetic_failures),
        }
        failures.extend(synthetic_failures)

    if args.source is not None:
        summary["execution"] = {
            "source": display_path(args.source),
            "emit_prefix": args.emit_prefix,
            "work_key": execution_work_key,
            "routing": execution_routing,
            "commands": [
                {
                    "role": result.role,
                    "command": result.command,
                    "exit_code": result.exit_code,
                    "stdout": result.stdout,
                    "stderr": result.stderr,
                }
                for result in execution_results
            ],
        }

    if args.golden_summary is not None:
        golden_path = args.golden_summary
        summary["golden_summary"] = display_path(golden_path)
        if args.write_golden:
            write_json(golden_path, summary)
            print(f"golden-updated: {display_path(golden_path)}")
        elif args.check_golden:
            if not golden_path.exists():
                failures.append(
                    "golden summary missing: "
                    f"{display_path(golden_path)} (run with --write-golden to create it)"
                )
            else:
                try:
                    expected = json.loads(golden_path.read_text(encoding="utf-8"))
                except json.JSONDecodeError as exc:
                    failures.append(
                        f"golden summary parse error at {display_path(golden_path)}: {exc}"
                    )
                else:
                    if expected != summary:
                        observed_digest = sha256_text(canonical_json(summary))
                        expected_digest = sha256_text(canonical_json(expected))
                        failures.append(
                            "golden summary drift detected: "
                            f"expected_sha256={expected_digest[:16]} "
                            f"observed_sha256={observed_digest[:16]} "
                            f"(update with --write-golden after intended contract changes)"
                        )

    summary["ok"] = not failures
    write_json(args.summary_out, summary)

    if failures:
        for failure in failures:
            print(f"PARITY-FAIL: {failure}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    compared_dimensions = sum(
        1 for item in dimension_results if item["status"] == "compared"
    )
    print(
        "PARITY-PASS: "
        f"compared {len(comparisons)} artifact(s), dimensions={compared_dimensions}"
    )
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


__all__ = ["DEFAULT_ARTIFACTS", "MODE", "main", "parse_args", "run"]
