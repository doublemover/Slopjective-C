#!/usr/bin/env python3
"""Build and contract-check script for generated site/index.md."""

from __future__ import annotations

import argparse
import difflib
import sys
from pathlib import Path

from build_pages import parse_toc, stitch, validate_files

ROOT = Path(__file__).resolve().parents[1]
SPEC_DIR = ROOT / "spec"
TOC_PATH = SPEC_DIR / "TABLE_OF_CONTENTS.md"
OUTPUT_PATH = ROOT / "site" / "index.md"
POLICY_README_PATH = ROOT / "site" / "src" / "README.md"

FRONT_MATTER = "\n".join(
    [
        "---",
        "title: Objective-C 3.0 Draft Specification",
        "layout: default",
        "---",
        "",
    ]
)

REQUIRED_POLICY_TOKENS: tuple[str, ...] = (
    "`site/index.md` is generated output",
    "Manual edits are unsupported.",
    "`python scripts/build_site_index.py`",
    "`python scripts/build_site_index.py --check-contract`",
    "`spec/TABLE_OF_CONTENTS.md`",
)


def render_expected() -> tuple[str, int]:
    names = parse_toc(TOC_PATH)
    paths = validate_files(names, SPEC_DIR)
    text = FRONT_MATTER + stitch(paths)
    return text, len(paths)


def validate_policy_readme() -> list[str]:
    errors: list[str] = []
    if not POLICY_README_PATH.is_file():
        return [f"missing policy README: {POLICY_README_PATH}"]

    text = POLICY_README_PATH.read_text(encoding="utf-8")
    for token in REQUIRED_POLICY_TOKENS:
        if token not in text:
            errors.append(f"policy README missing token: {token}")
    return errors


def format_diff(actual: str, expected: str) -> str:
    diff_lines = list(
        difflib.unified_diff(
            actual.splitlines(),
            expected.splitlines(),
            fromfile="site/index.md (actual)",
            tofile="site/index.md (expected)",
            lineterm="",
        )
    )
    if not diff_lines:
        return ""
    preview = diff_lines[:60]
    return "\n".join(preview)


def check_contract() -> int:
    errors = validate_policy_readme()
    if errors:
        print("site-index-contract: FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    if not OUTPUT_PATH.is_file():
        print("site-index-contract: FAIL", file=sys.stderr)
        print(f"- missing generated output: {OUTPUT_PATH}", file=sys.stderr)
        return 1

    expected, count = render_expected()
    actual = OUTPUT_PATH.read_text(encoding="utf-8")
    if actual != expected:
        print("site-index-contract: FAIL", file=sys.stderr)
        print(
            "- site/index.md drift detected. This file is generated-only; "
            "manual edits are unsupported.",
            file=sys.stderr,
        )
        print(
            "- Regenerate with: python scripts/build_site_index.py",
            file=sys.stderr,
        )
        diff_preview = format_diff(actual, expected)
        if diff_preview:
            print("- Diff preview:", file=sys.stderr)
            print(diff_preview, file=sys.stderr)
        return 1

    print(f"site-index-contract: OK (documents={count}, output={OUTPUT_PATH})")
    return 0


def build_index() -> int:
    expected, count = render_expected()
    OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_PATH.write_text(expected, encoding="utf-8")
    print(f"site-index-build: wrote {OUTPUT_PATH} ({count} documents)")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="build_site_index.py",
        description=(
            "Build or validate generated site/index.md from deterministic "
            "spec inputs."
        ),
    )
    parser.add_argument(
        "--check-contract",
        action="store_true",
        help="Fail if generated-only site/index.md drifts from canonical inputs.",
    )
    return parser


def main() -> int:
    args = build_parser().parse_args()
    if args.check_contract:
        return check_contract()
    return build_index()


if __name__ == "__main__":
    raise SystemExit(main())
