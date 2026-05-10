from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from .constants import FRAGMENT_ORDER, OUTPUT_PATH
from .output_writing import output_digest, read_output_bytes, write_output_bytes
from .section_rendering import print_diff_preview
from .source_loading import (
    required_fragment_paths,
    stitch_fragments,
    validate_source_contract,
)


def check_contract(*, allow_missing_fragments: bool) -> tuple[int, list[Path]]:
    result = validate_source_contract(allow_missing_fragments=allow_missing_fragments)

    if result.errors:
        print("objc3c-native-docs-contract: FAIL", file=sys.stderr)
        for error in result.errors:
            print(f"- {error}", file=sys.stderr)
        print(
            "- Guidance: ensure only canonical fragment files exist and follow README order.",
            file=sys.stderr,
        )
        return 1, result.required_paths

    print(
        "objc3c-native-docs-contract: OK "
        f"(order={len(FRAGMENT_ORDER)}, "
        f"present={len(result.required_paths)}, "
        f"missing={len(result.missing_fragments)})"
    )
    for warning in result.warnings:
        print(f"objc3c-native-docs-contract: WARN {warning}")
    return 0, result.required_paths


def build_docs() -> int:
    contract_status, paths = check_contract(allow_missing_fragments=True)
    if contract_status != 0:
        return contract_status

    if not paths:
        print(
            "objc3c-native-docs-build: no source fragments found; "
            "leaving docs/objc3c-native.md unchanged."
        )
        return 0

    _, missing = required_fragment_paths()
    if missing:
        print(
            "objc3c-native-docs-build: FAIL missing required fragments: "
            + ", ".join(missing),
            file=sys.stderr,
        )
        print(
            "objc3c-native-docs-build: Guidance: create all canonical "
            "fragment files before stitching.",
            file=sys.stderr,
        )
        return 1

    stitched = stitch_fragments(paths)
    existing = read_output_bytes()
    if existing == stitched:
        print(
            "objc3c-native-docs-build: up-to-date "
            f"(sha256={output_digest(stitched)})"
        )
        return 0

    write_output_bytes(stitched)
    print(
        "objc3c-native-docs-build: wrote "
        f"{OUTPUT_PATH} (sha256={output_digest(stitched)})"
    )
    return 0


def check_drift() -> int:
    contract_status, paths = check_contract(allow_missing_fragments=False)
    if contract_status != 0:
        return contract_status

    expected = stitch_fragments(paths)
    if not OUTPUT_PATH.is_file():
        print("objc3c-native-docs-check: FAIL", file=sys.stderr)
        print(f"- missing generated output: {OUTPUT_PATH}", file=sys.stderr)
        print(
            "- Regenerate with: python scripts/build_objc3c_native_docs.py",
            file=sys.stderr,
        )
        return 1

    actual = OUTPUT_PATH.read_bytes()
    if actual != expected:
        print("objc3c-native-docs-check: FAIL", file=sys.stderr)
        print(
            "- docs/objc3c-native.md drift detected against canonical fragments.",
            file=sys.stderr,
        )
        print(
            "- Regenerate with: python scripts/build_objc3c_native_docs.py",
            file=sys.stderr,
        )
        print_diff_preview(actual, expected)
        return 1

    print(
        "objc3c-native-docs-check: OK "
        f"(fragments={len(paths)}, sha256={output_digest(expected)})"
    )
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="build_objc3c_native_docs.py",
        description=(
            "Validate or build docs/objc3c-native.md from deterministic source "
            "fragments under docs/objc3c-native/src."
        ),
    )
    parser.add_argument(
        "--check-contract",
        action="store_true",
        help=(
            "Validate deterministic source fragment and stitch-order contract "
            "(migration-safe; allows missing fragment files)."
        ),
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help=(
            "Validate deterministic source contract and fail on generated "
            "output drift."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.check:
        return check_drift()
    if args.check_contract:
        status, _ = check_contract(allow_missing_fragments=True)
        return status
    return build_docs()
