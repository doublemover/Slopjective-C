from __future__ import annotations

import argparse
import json
import sys
from typing import Sequence

from conformance_evidence_index.builder import (
    build_artifact_records,
    build_index_payload,
)
from conformance_evidence_index.constants import (
    DEFAULT_GLOBS,
    DEFAULT_INPUT_ROOT,
    GENERATOR_PATH,
)
from conformance_evidence_index.paths import (
    collect_artifact_paths,
    normalize_pattern_list,
    normalize_repo_path,
    resolve_repo_path,
)
from conformance_evidence_index.timestamps import (
    StrictGeneratedAtError,
    resolve_index_generated_at,
)


def build_parser() -> argparse.ArgumentParser:
    sample_output = "/".join(("reports", "conformance", "evidence-index.v0.11.sample.json"))
    release_output = "/".join(("reports", "conformance", "evidence-index.v0.11.json"))
    default_input_root = "/".join(("reports", "conformance"))
    parser = argparse.ArgumentParser(
        prog="generate_conformance_evidence_index.py",
        description=(
            "Scan conformance evidence artifacts and emit a deterministic "
            "profile/release index JSON."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "Examples:\n"
            f"  {GENERATOR_PATH} \\\n"
            f"    --output {sample_output} \\\n"
            "    --release-label v0.11 \\\n"
            "    --generated-at 2026-02-23T00:00:00Z\n\n"
            f"  SOURCE_DATE_EPOCH=1767139200 {GENERATOR_PATH} \\\n"
            f"    --output {release_output} \\\n"
            "    --release-label v0.11"
        ),
    )
    parser.add_argument(
        "--input-root",
        default=str(DEFAULT_INPUT_ROOT),
        help=(
            "Artifact root directory to scan. Defaults to "
            f"'{default_input_root}' (relative to repository root)."
        ),
    )
    parser.add_argument(
        "--glob",
        action="append",
        default=None,
        help=(
            "Glob pattern(s) under --input-root to include. May be repeated. "
            "Defaults to '**/*.json'."
        ),
    )
    parser.add_argument(
        "--exclude",
        action="append",
        default=None,
        help=(
            "Repository-relative file path(s) to exclude from indexing. "
            "May be repeated."
        ),
    )
    parser.add_argument(
        "--output",
        default="-",
        help=(
            "Output path for index JSON, relative to repository root. "
            "Use '-' to write to stdout (default)."
        ),
    )
    parser.add_argument(
        "--release-label",
        default=None,
        help=(
            "Optional index-level release label (for example: v0.11). "
            "Also used as retired route release_id when an artifact has no release token."
        ),
    )
    parser.add_argument(
        "--generated-at",
        default=None,
        help=(
            "Optional RFC3339 timestamp (UTC) to embed in the index. "
            "If omitted, SOURCE_DATE_EPOCH is used when set."
        ),
    )
    parser.add_argument(
        "--strict-generated-at",
        action="store_true",
        help=(
            "Fail with exit code 2 when an artifact generated_at value is not "
            "valid RFC3339 with timezone."
        ),
    )
    parser.add_argument(
        "--allow-empty",
        action="store_true",
        help="Allow writing an empty index when no artifacts match.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    input_root = resolve_repo_path(args.input_root)
    if not input_root.exists():
        parser.error(f"--input-root does not exist: {normalize_repo_path(input_root)}")
    if not input_root.is_dir():
        parser.error(f"--input-root is not a directory: {normalize_repo_path(input_root)}")

    globs = normalize_pattern_list(args.glob, DEFAULT_GLOBS)
    excluded_paths: set = set()
    for raw_exclusion in normalize_pattern_list(args.exclude, ()):
        excluded_paths.add(resolve_repo_path(raw_exclusion))

    output_path = None
    if args.output != "-":
        output_path = resolve_repo_path(args.output)
        if output_path in excluded_paths:
            pass
        elif output_path.is_relative_to(input_root):
            excluded_paths.add(output_path)

    try:
        generated_at = resolve_index_generated_at(args.generated_at)
    except ValueError as exc:
        parser.error(str(exc))

    artifact_paths = collect_artifact_paths(
        input_root=input_root,
        globs=globs,
        excluded_paths=excluded_paths,
    )
    if not artifact_paths and not args.allow_empty:
        parser.error(
            "no artifacts matched; check --input-root/--glob or pass --allow-empty"
        )

    try:
        records = build_artifact_records(
            artifact_paths=artifact_paths,
            release_retired_route=args.release_label,
            strict_generated_at=args.strict_generated_at,
        )
    except StrictGeneratedAtError as exc:
        parser.error(str(exc))
    payload = build_index_payload(
        records=records,
        input_root=input_root,
        output_path=output_path,
        release_label=args.release_label,
        generated_at=generated_at,
    )
    rendered = json.dumps(payload, indent=2, ensure_ascii=True) + "\n"

    if output_path is None:
        sys.stdout.write(rendered)
        return 0

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(rendered, encoding="utf-8")
    return 0
