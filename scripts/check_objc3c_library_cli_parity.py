#!/usr/bin/env python3
"""Compare library and CLI artifact outputs for deterministic parity."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
TMP_ROOT = ROOT / "tmp"
MODE = "objc3c-library-cli-parity-v2"
DEFAULT_ARTIFACTS = (
    "module.diagnostics.json",
    "module.manifest.json",
    "module.ll",
    "module.o",
)
DEFAULT_DIMENSION_MAP = {
    "diagnostics": "module.diagnostics.json",
    "manifest": "module.manifest.json",
    "ir": "module.ll",
    "object": "module.o",
}
DIMENSION_ORDER = ("diagnostics", "manifest", "ir", "object")
SHA256_PATTERN = re.compile(r"^[0-9a-f]{64}$")


@dataclass(frozen=True)
class ArtifactDigest:
    source_kind: str
    source_path: str
    sha256: str


@dataclass(frozen=True)
class CommandResult:
    role: str
    command: list[str]
    exit_code: int
    stdout: str
    stderr: str


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def sha256_hex(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        while True:
            chunk = handle.read(1024 * 1024)
            if not chunk:
                break
            digest.update(chunk)
    return digest.hexdigest()


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
        default=Path("tmp/objc3c_library_cli_parity_work"),
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
        help="llc path forwarded to CLI in --source mode",
    )
    parser.add_argument(
        "--cli-ir-object-backend",
        choices=("clang", "llvm-direct"),
        default="clang",
        help="IR object backend for CLI command in --source mode",
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


def ensure_directory(path: Path, *, label: str) -> None:
    if not path.exists():
        raise ValueError(f"{label} does not exist: {display_path(path)}")
    if not path.is_dir():
        raise ValueError(f"{label} must be a directory: {display_path(path)}")


def ensure_file(path: Path, *, label: str) -> None:
    if not path.exists():
        raise ValueError(f"{label} does not exist: {display_path(path)}")
    if not path.is_file():
        raise ValueError(f"{label} must be a file: {display_path(path)}")


def ensure_under_tmp(path: Path, *, label: str) -> None:
    try:
        resolved = path.resolve()
    except OSError:
        resolved = path
    try:
        resolved.relative_to(TMP_ROOT.resolve())
    except ValueError as exc:
        raise ValueError(
            f"{label} must be under {display_path(TMP_ROOT)}: {display_path(path)}"
        ) from exc


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def default_dimension_map_for_emit_prefix(*, emit_prefix: str, object_artifact: str) -> dict[str, str]:
    return {
        "diagnostics": f"{emit_prefix}.diagnostics.json",
        "manifest": f"{emit_prefix}.manifest.json",
        "ir": f"{emit_prefix}.ll",
        "object": object_artifact,
    }


def parse_dimension_map(
    values: Sequence[str],
    *,
    default_mapping: dict[str, str],
) -> dict[str, str]:
    mapping = dict(default_mapping)
    for raw in values:
        if "=" not in raw:
            raise ValueError(
                f"--dimension-map entry must use DIMENSION=ARTIFACT format: {raw!r}"
            )
        dimension, artifact = raw.split("=", 1)
        dimension = dimension.strip()
        artifact = artifact.strip()
        if dimension not in DEFAULT_DIMENSION_MAP:
            supported = ", ".join(DIMENSION_ORDER)
            raise ValueError(
                f"unsupported dimension {dimension!r}; expected one of: {supported}"
            )
        if not artifact:
            raise ValueError(
                f"--dimension-map artifact path must be non-empty for {dimension!r}"
            )
        mapping[dimension] = artifact
    return mapping


def normalize_artifacts(values: Sequence[str]) -> list[str]:
    normalized = sorted({value.strip() for value in values if value and value.strip()})
    if not normalized:
        raise ValueError("--artifacts must include at least one artifact filename")
    return normalized


def parse_proxy_digest(path: Path) -> str:
    text = path.read_text(encoding="utf-8").strip().lower()
    if not SHA256_PATTERN.fullmatch(text):
        raise ValueError(
            f"invalid sha256 proxy digest in {display_path(path)}; expected 64 lowercase hex chars"
        )
    return text


def resolve_artifact_digest(
    *,
    base_dir: Path,
    artifact_name: str,
) -> ArtifactDigest:
    artifact_path = base_dir / artifact_name
    if artifact_path.exists():
        if not artifact_path.is_file():
            raise ValueError(
                f"artifact path must be a file: {display_path(artifact_path)}"
            )
        return ArtifactDigest(
            source_kind="artifact-bytes",
            source_path=display_path(artifact_path),
            sha256=sha256_hex(artifact_path),
        )

    proxy_path = base_dir / f"{artifact_name}.sha256"
    if proxy_path.exists():
        if not proxy_path.is_file():
            raise ValueError(
                f"proxy digest path must be a file: {display_path(proxy_path)}"
            )
        return ArtifactDigest(
            source_kind="sha256-proxy",
            source_path=display_path(proxy_path),
            sha256=parse_proxy_digest(proxy_path),
        )

    raise ValueError(
        "missing artifact and proxy digest: "
        f"{display_path(artifact_path)} (or {display_path(proxy_path)})"
    )


def build_dimension_results(
    *,
    artifacts: Sequence[str],
    dimension_map: dict[str, str],
) -> list[dict[str, str]]:
    artifact_set = set(artifacts)
    results: list[dict[str, str]] = []
    for dimension in DIMENSION_ORDER:
        artifact = dimension_map[dimension]
        if artifact in artifact_set:
            status = "compared"
        else:
            status = "skipped"
        results.append(
            {
                "dimension": dimension,
                "artifact": artifact,
                "status": status,
            }
        )
    return results


def write_json(path: Path, payload: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(canonical_json(payload), encoding="utf-8")


def format_command(command: Sequence[str]) -> str:
    return subprocess.list2cmdline([str(part) for part in command])


def run_command(role: str, command: Sequence[str]) -> CommandResult:
    completed = subprocess.run(
        [str(part) for part in command],
        capture_output=True,
        text=True,
        check=False,
    )
    return CommandResult(
        role=role,
        command=[str(part) for part in command],
        exit_code=completed.returncode,
        stdout=completed.stdout,
        stderr=completed.stderr,
    )


def build_source_mode_artifacts(*, emit_prefix: str) -> list[str]:
    return [
        f"{emit_prefix}.diagnostics.json",
        f"{emit_prefix}.manifest.json",
        f"{emit_prefix}.ll",
        f"{emit_prefix}.obj",
    ]


def prepare_source_mode(
    args: argparse.Namespace,
) -> tuple[Path, Path, str, list[CommandResult], list[str]]:
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
        work_key = sha256_text(
            f"{display_path(args.source)}|{args.emit_prefix}"
        )[:16]
    work_root = work_dir / work_key

    library_dir = args.library_dir if args.library_dir is not None else work_root / "library"
    cli_dir = args.cli_dir if args.cli_dir is not None else work_root / "cli"
    if not args.allow_non_tmp_work_dir:
        ensure_under_tmp(library_dir, label="library-dir")
        ensure_under_tmp(cli_dir, label="cli-dir")
    library_dir.mkdir(parents=True, exist_ok=True)
    cli_dir.mkdir(parents=True, exist_ok=True)

    cli_command: list[str] = [
        str(args.cli_bin),
        str(args.source),
        "--out-dir",
        str(cli_dir),
        "--emit-prefix",
        args.emit_prefix,
        "--objc3-ir-object-backend",
        args.cli_ir_object_backend,
    ]
    if args.clang_path is not None:
        cli_command.extend(["--clang", str(args.clang_path)])
    if args.llc_path is not None:
        cli_command.extend(["--llc", str(args.llc_path)])
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
    ]
    if args.clang_path is not None:
        c_api_command.extend(["--clang", str(args.clang_path)])
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
    return library_dir, cli_dir, work_key, results, failures


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    if args.check_golden and args.write_golden:
        raise ValueError("--check-golden and --write-golden cannot be used together")
    if (args.check_golden or args.write_golden) and args.golden_summary is None:
        raise ValueError("--golden-summary is required when using --check-golden/--write-golden")

    execution_results: list[CommandResult] = []
    execution_failures: list[str] = []
    execution_work_key: str | None = None
    if args.source is not None:
        (
            library_dir,
            cli_dir,
            execution_work_key,
            execution_results,
            execution_failures,
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

    if args.source is not None:
        summary["execution"] = {
            "source": display_path(args.source),
            "emit_prefix": args.emit_prefix,
            "work_key": execution_work_key,
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


if __name__ == "__main__":
    main()
