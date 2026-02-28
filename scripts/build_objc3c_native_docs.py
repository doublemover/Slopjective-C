#!/usr/bin/env python3
"""Contract-aware stitcher for docs/objc3c-native fragments."""

from __future__ import annotations

import argparse
import difflib
import hashlib
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
LINT_DISABLE_LINE = b"<!-- markdownlint-disable-file MD041 -->"


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
    known = set(FRAGMENT_ORDER) | {"README.md", "OWNERSHIP.md"}
    unknown = [
        path.name
        for path in sorted(SRC_DIR.glob("*.md"))
        if path.name not in known
    ]
    return unknown


def required_fragment_paths() -> tuple[list[Path], list[str]]:
    paths: list[Path] = []
    missing: list[str] = []
    for name in FRAGMENT_ORDER:
        path = SRC_DIR / name
        if path.is_file():
            paths.append(path)
        else:
            missing.append(name)
    return paths, missing


def check_contract(*, allow_missing_fragments: bool) -> tuple[int, list[Path]]:
    errors: list[str] = []
    warnings: list[str] = []

    if not README_PATH.is_file():
        errors.append(f"missing contract README: {README_PATH}")
    else:
        readme_text = README_PATH.read_text(encoding="utf-8")
        errors.extend(validate_readme_contract(readme_text))

    required_paths, missing = required_fragment_paths()
    if missing and not allow_missing_fragments:
        errors.append(
            "missing required fragment files: "
            + ", ".join(missing)
        )
    elif missing:
        warnings.append(
            "missing fragments tolerated in --check-contract mode: "
            + ", ".join(missing)
        )

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
        print("- Guidance: ensure only canonical fragment files exist and follow README order.", file=sys.stderr)
        return 1, required_paths

    print(
        "objc3c-native-docs-contract: OK "
        f"(order={len(FRAGMENT_ORDER)}, present={len(required_paths)}, missing={len(missing)})"
    )
    for warning in warnings:
        print(f"objc3c-native-docs-contract: WARN {warning}")
    return 0, required_paths


def read_fragment_bytes(path: Path) -> bytes:
    data = path.read_bytes()
    if data.startswith(LINT_DISABLE_LINE):
        first_newline = data.find(b"\n")
        if first_newline != -1:
            data = data[first_newline + 1 :]
            if data.startswith(b"\r\n"):
                data = data[2:]
            elif data.startswith(b"\n"):
                data = data[1:]
    if not data.endswith((b"\n", b"\r")):
        data += b"\n"
    return data


def stitch_fragments(paths: list[Path]) -> bytes:
    return b"".join(read_fragment_bytes(path) for path in paths)


def print_diff_preview(actual: bytes, expected: bytes) -> None:
    actual_text = actual.decode("utf-8", errors="replace")
    expected_text = expected.decode("utf-8", errors="replace")
    diff = list(
        difflib.unified_diff(
            actual_text.splitlines(),
            expected_text.splitlines(),
            fromfile="docs/objc3c-native.md (actual)",
            tofile="docs/objc3c-native.md (expected)",
            lineterm="",
        )
    )
    if not diff:
        return
    print("- Diff preview:", file=sys.stderr)
    for line in diff[:120]:
        print(line, file=sys.stderr)


def output_digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()[:16]


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
    existing = OUTPUT_PATH.read_bytes() if OUTPUT_PATH.is_file() else b""
    if existing == stitched:
        print(
            "objc3c-native-docs-build: up-to-date "
            f"(sha256={output_digest(stitched)})"
        )
        return 0

    OUTPUT_PATH.write_bytes(stitched)
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


def main() -> int:
    args = build_parser().parse_args()
    if args.check:
        return check_drift()
    if args.check_contract:
        status, _ = check_contract(allow_missing_fragments=True)
        return status
    return build_docs()


if __name__ == "__main__":
    raise SystemExit(main())
