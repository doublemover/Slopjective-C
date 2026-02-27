#!/usr/bin/env python3
"""Contract-aware stitcher for docs/objc3c-native fragments."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC_DIR = ROOT / "docs" / "objc3c-native" / "src"
README_PATH = SRC_DIR / "README.md"
OUTPUT_PATH = ROOT / "docs" / "objc3c-native.md"

FRAGMENT_ORDER: tuple[str, ...] = (
    "10-cli.md",
    "20-grammar.md",
    "30-semantics.md",
    "40-diagnostics.md",
    "50-artifacts.md",
    "60-tests.md",
)
REQUIRED_HEADINGS: tuple[str, ...] = (
    "## Ownership Policy",
    "## Canonical Fragment Taxonomy",
    "## Deterministic Stitch Order",
    "## Include Rules",
    "## Contract Validation",
)
ORDER_LINE_RE = re.compile(r"^\d+\.\s+`([^`]+\.md)`\s*$")


def parse_order_from_readme(text: str) -> list[str]:
    observed: list[str] = []
    for raw_line in text.splitlines():
        match = ORDER_LINE_RE.match(raw_line.strip())
        if match:
            observed.append(match.group(1))
    return observed


def validate_readme_contract(text: str) -> list[str]:
    errors: list[str] = []

    for heading in REQUIRED_HEADINGS:
        if heading not in text:
            errors.append(f"missing required README heading: {heading}")

    for fragment in FRAGMENT_ORDER:
        token = f"`{fragment}`"
        if token not in text:
            errors.append(f"README missing fragment reference: {token}")

    observed_order = parse_order_from_readme(text)
    if observed_order != list(FRAGMENT_ORDER):
        errors.append(
            "README stitch order drift: "
            f"expected {list(FRAGMENT_ORDER)} observed {observed_order}"
        )

    if len(observed_order) != len(set(observed_order)):
        errors.append("README stitch order contains duplicate fragment entries")

    return errors


def find_unknown_fragments() -> list[str]:
    known = set(FRAGMENT_ORDER) | {"README.md"}
    unknown = [
        path.name
        for path in sorted(SRC_DIR.glob("*.md"))
        if path.name not in known
    ]
    return unknown


def check_contract() -> int:
    errors: list[str] = []

    if not README_PATH.is_file():
        errors.append(f"missing contract README: {README_PATH}")
    else:
        readme_text = README_PATH.read_text(encoding="utf-8")
        errors.extend(validate_readme_contract(readme_text))

    unknown_fragments = find_unknown_fragments()
    if unknown_fragments:
        errors.append(
            "unexpected fragments in docs source dir: "
            + ", ".join(unknown_fragments)
        )

    if errors:
        print("objc3c-native-docs-contract: FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    present = [name for name in FRAGMENT_ORDER if (SRC_DIR / name).is_file()]
    print(
        "objc3c-native-docs-contract: OK "
        f"(order={len(FRAGMENT_ORDER)}, present={len(present)})"
    )
    return 0


def stitch_fragments() -> str:
    sections: list[str] = []
    for name in FRAGMENT_ORDER:
        path = SRC_DIR / name
        text = path.read_text(encoding="utf-8").rstrip("\n")
        header = f"<!-- BEGIN src/{name} -->"
        footer = f"<!-- END src/{name} -->"
        sections.append(f"{header}\n{text}\n{footer}")
    return "\n\n---\n\n".join(sections) + "\n"


def build_docs() -> int:
    contract_status = check_contract()
    if contract_status != 0:
        return contract_status

    present = [name for name in FRAGMENT_ORDER if (SRC_DIR / name).is_file()]
    if not present:
        print(
            "objc3c-native-docs-build: no source fragments found; "
            "leaving docs/objc3c-native.md unchanged."
        )
        return 0

    missing = [name for name in FRAGMENT_ORDER if name not in present]
    if missing:
        print(
            "objc3c-native-docs-build: FAIL missing required fragments: "
            + ", ".join(missing),
            file=sys.stderr,
        )
        return 1

    stitched = stitch_fragments()
    OUTPUT_PATH.write_text(stitched, encoding="utf-8")
    print(f"objc3c-native-docs-build: wrote {OUTPUT_PATH}")
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
        help="Validate deterministic source fragment and stitch-order contract.",
    )
    return parser


def main() -> int:
    args = build_parser().parse_args()
    if args.check_contract:
        return check_contract()
    return build_docs()


if __name__ == "__main__":
    raise SystemExit(main())
