#!/usr/bin/env python3
"""Build and contract-check script for generated site/index.md."""

from __future__ import annotations

import argparse
import difflib
import hashlib
import json
import sys
from pathlib import Path

from build_pages import parse_toc, stitch, validate_files

ROOT = Path(__file__).resolve().parents[1]
SRC_DIR = ROOT / "site" / "src"
CONFIG_PATH = SRC_DIR / "index.contract.json"
POLICY_README_PATH = ROOT / "site" / "src" / "README.md"
ALLOWED_SRC_FILES: set[str] = {"README.md", "index.contract.json", "OWNERSHIP.md"}

REQUIRED_POLICY_TOKENS: tuple[str, ...] = (
    "`site/index.md` is generated output",
    "Manual edits are unsupported.",
    "`python scripts/build_site_index.py`",
    "`python scripts/build_site_index.py --check`",
    "`site/src/index.contract.json`",
    "`spec/TABLE_OF_CONTENTS.md`",
)


class ContractConfig:
    def __init__(
        self,
        *,
        spec_dir: Path,
        toc_path: Path,
        output_path: Path,
        front_matter: str,
    ) -> None:
        self.spec_dir = spec_dir
        self.toc_path = toc_path
        self.output_path = output_path
        self.front_matter = front_matter


def load_contract_config() -> tuple[ContractConfig | None, list[str]]:
    errors: list[str] = []
    if not CONFIG_PATH.is_file():
        return None, [f"missing config file: {CONFIG_PATH}"]

    try:
        payload = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        return None, [f"invalid JSON in {CONFIG_PATH}: {exc}"]

    if not isinstance(payload, dict):
        return None, [f"config root must be an object: {CONFIG_PATH}"]

    if payload.get("contract_id") != "site-index-generator/v1":
        errors.append(
            "config contract_id drift: expected 'site-index-generator/v1' "
            f"observed {payload.get('contract_id')!r}"
        )

    def get_path_field(key: str) -> Path | None:
        value = payload.get(key)
        if not isinstance(value, str) or not value.strip():
            errors.append(f"config field {key!r} must be a non-empty string")
            return None
        return ROOT / value

    output_path = get_path_field("output_path")
    spec_dir = get_path_field("spec_dir")
    toc_path = get_path_field("toc_path")

    front_matter_lines = payload.get("front_matter")
    front_matter = ""
    if not isinstance(front_matter_lines, list) or not front_matter_lines:
        errors.append("config field 'front_matter' must be a non-empty string array")
    else:
        parsed_lines: list[str] = []
        for index, value in enumerate(front_matter_lines):
            if not isinstance(value, str):
                errors.append(
                    f"config field 'front_matter[{index}]' must be a string"
                )
                continue
            parsed_lines.append(value)
        if parsed_lines:
            front_matter = "\n".join(parsed_lines)
            if not front_matter.endswith("\n"):
                front_matter += "\n"

    if errors or output_path is None or spec_dir is None or toc_path is None:
        return None, errors

    return (
        ContractConfig(
            spec_dir=spec_dir,
            toc_path=toc_path,
            output_path=output_path,
            front_matter=front_matter,
        ),
        [],
    )


def find_unknown_src_files() -> list[str]:
    unknown: list[str] = []
    if not SRC_DIR.is_dir():
        return [str(SRC_DIR)]
    for path in sorted(SRC_DIR.glob("*")):
        if not path.is_file():
            continue
        if path.name not in ALLOWED_SRC_FILES:
            unknown.append(path.name)
    return unknown


def render_expected(config: ContractConfig) -> tuple[str, int]:
    names = parse_toc(config.toc_path)
    paths = validate_files(names, config.spec_dir)
    text = config.front_matter + stitch(paths)
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


def digest(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()[:16]


def validate_contract_inputs() -> tuple[ContractConfig | None, list[str]]:
    errors = validate_policy_readme()
    unknown_src_files = find_unknown_src_files()
    if unknown_src_files:
        errors.append(
            "unexpected files under site/src: " + ", ".join(unknown_src_files)
        )

    config, config_errors = load_contract_config()
    errors.extend(config_errors)
    return config, errors


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
            "- Regenerate with: python scripts/build_site_index.py",
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
            "- Regenerate with: python scripts/build_site_index.py",
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
    existing = config.output_path.read_text(encoding="utf-8") if config.output_path.is_file() else ""
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
    parser.add_argument(
        "--check-contract",
        action="store_true",
        help="Deprecated alias for --check.",
    )
    return parser


def main() -> int:
    args = build_parser().parse_args()
    if args.check or args.check_contract:
        if args.check_contract and not args.check:
            print(
                "site-index-check: WARN --check-contract is deprecated; use --check.",
                file=sys.stderr,
            )
        return check_drift()
    return build_index()


if __name__ == "__main__":
    raise SystemExit(main())
