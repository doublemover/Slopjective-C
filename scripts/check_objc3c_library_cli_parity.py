#!/usr/bin/env python3
"""Compare library and CLI artifact outputs for deterministic parity."""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]


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
        default=["module.diagnostics.json", "module.manifest.json", "module.ll", "module.o"],
        help="artifact filenames to compare relative to library/cli directories",
    )
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/objc3c_library_cli_parity_summary.json"),
        help="write summary JSON report to this path",
    )
    return parser.parse_args(argv)


def ensure_directory(path: Path, *, label: str) -> None:
    if not path.exists():
        raise ValueError(f"{label} does not exist: {display_path(path)}")
    if not path.is_dir():
        raise ValueError(f"{label} must be a directory: {display_path(path)}")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    ensure_directory(args.library_dir, label="library-dir")
    ensure_directory(args.cli_dir, label="cli-dir")

    failures: list[str] = []
    comparisons: list[dict[str, str | bool]] = []
    for artifact_name in args.artifacts:
        library_path = args.library_dir / artifact_name
        cli_path = args.cli_dir / artifact_name
        if not library_path.exists():
            failures.append(f"missing library artifact: {display_path(library_path)}")
            continue
        if not cli_path.exists():
            failures.append(f"missing cli artifact: {display_path(cli_path)}")
            continue

        library_digest = sha256_hex(library_path)
        cli_digest = sha256_hex(cli_path)
        matches = library_digest == cli_digest
        if not matches:
            failures.append(
                f"digest mismatch for {artifact_name}: "
                f"library={library_digest[:16]} cli={cli_digest[:16]}"
            )
        comparisons.append(
            {
                "artifact": artifact_name,
                "library_path": display_path(library_path),
                "cli_path": display_path(cli_path),
                "library_sha256": library_digest,
                "cli_sha256": cli_digest,
                "matches": matches,
            }
        )

    summary = {
        "ok": not failures,
        "library_dir": display_path(args.library_dir),
        "cli_dir": display_path(args.cli_dir),
        "comparisons": comparisons,
        "failures": failures,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"PARITY-FAIL: {failure}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print(f"PARITY-PASS: compared {len(comparisons)} artifact(s)")
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()
