from __future__ import annotations

import argparse
import sys

from .rendering import digest, format_diff, render_expected
from .validation import validate_contract_inputs


def check_drift() -> int:
    config, errors = validate_contract_inputs()
    if errors or config is None:
        print("site-index-check: FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        print(
            "- Guidance: ensure site/src/README.md and "
            "site/src/index.contract.json are present and valid.",
            file=sys.stderr,
        )
        return 1

    if not config.output_path.is_file():
        print("site-index-check: FAIL", file=sys.stderr)
        print(f"- missing generated output: {config.output_path}", file=sys.stderr)
        print(
            "- Regenerate with: npm run objc3c -- build-site",
            file=sys.stderr,
        )
        return 1

    expected, count = render_expected(config)
    actual = config.output_path.read_text(encoding="utf-8")
    if actual != expected:
        print("site-index-check: FAIL", file=sys.stderr)
        print(
            "- site/index.md drift detected. This file is generated-only; "
            "manual edits are unsupported.",
            file=sys.stderr,
        )
        print(
            "- Regenerate with: npm run objc3c -- build-site",
            file=sys.stderr,
        )
        diff_preview = format_diff(actual, expected)
        if diff_preview:
            print("- Diff preview:", file=sys.stderr)
            print(diff_preview, file=sys.stderr)
        return 1

    print(
        "site-index-check: OK "
        f"(documents={count}, output={config.output_path}, sha256={digest(expected)})"
    )
    return 0


def build_index() -> int:
    config, errors = validate_contract_inputs()
    if errors or config is None:
        print("site-index-build: FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    expected, count = render_expected(config)
    config.output_path.parent.mkdir(parents=True, exist_ok=True)
    existing = (
        config.output_path.read_text(encoding="utf-8")
        if config.output_path.is_file()
        else ""
    )
    if existing == expected:
        print(
            "site-index-build: up-to-date "
            f"(documents={count}, sha256={digest(expected)})"
        )
        return 0

    config.output_path.write_text(expected, encoding="utf-8")
    print(
        "site-index-build: wrote "
        f"{config.output_path} (documents={count}, sha256={digest(expected)})"
    )
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
        "--check",
        action="store_true",
        help="Fail if generated-only site/index.md drifts from canonical inputs.",
    )
    return parser


def main() -> int:
    args = build_parser().parse_args()
    if args.check:
        return check_drift()
    return build_index()
