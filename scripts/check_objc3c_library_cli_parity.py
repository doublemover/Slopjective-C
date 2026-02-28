#!/usr/bin/env python3
"""Compare library and CLI artifact outputs for deterministic parity."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
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
    parser.add_argument("--library-dir", type=Path, required=True)
    parser.add_argument("--cli-dir", type=Path, required=True)
    parser.add_argument(
        "--artifacts",
        nargs="+",
        default=list(DEFAULT_ARTIFACTS),
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


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def parse_dimension_map(values: Sequence[str]) -> dict[str, str]:
    mapping = dict(DEFAULT_DIMENSION_MAP)
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    ensure_directory(args.library_dir, label="library-dir")
    ensure_directory(args.cli_dir, label="cli-dir")
    if args.check_golden and args.write_golden:
        raise ValueError("--check-golden and --write-golden cannot be used together")
    if (args.check_golden or args.write_golden) and args.golden_summary is None:
        raise ValueError("--golden-summary is required when using --check-golden/--write-golden")

    artifacts = normalize_artifacts(args.artifacts)
    dimension_map = parse_dimension_map(args.dimension_map)

    failures: list[str] = []
    comparisons: list[dict[str, Any]] = []
    artifact_to_dimension = {
        artifact: dimension
        for dimension, artifact in dimension_map.items()
    }
    for artifact_name in artifacts:
        try:
            library_digest = resolve_artifact_digest(
                base_dir=args.library_dir,
                artifact_name=artifact_name,
            )
        except ValueError as exc:
            failures.append(f"library {artifact_name}: {exc}")
            continue
        try:
            cli_digest = resolve_artifact_digest(
                base_dir=args.cli_dir,
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

    summary = {
        "mode": MODE,
        "library_dir": display_path(args.library_dir),
        "cli_dir": display_path(args.cli_dir),
        "artifacts": artifacts,
        "dimensions": dimension_results,
        "comparisons": comparisons,
        "failures": failures,
        "ok": not failures,
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
