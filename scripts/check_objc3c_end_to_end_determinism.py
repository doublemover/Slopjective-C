#!/usr/bin/env python3
"""Fail-closed end-to-end determinism gate for repeated objc3c replay runs."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from objc3c_tooling.json_io import canonical_json
from objc3c_tooling.paths import display_path
from scripts.objc3c_end_to_end_determinism import (
    DEFAULT_REPLAY_ROOT,
    MAX_STDIO_PREVIEW_CHARS,
    MODE,
    DeterminismContractError,
    build_parser,
    check_determinism,
    collect_artifacts,
    compare_runs,
    expand_command_tokens,
    normalize_command_tokens,
    parse_key_value,
    parse_variant_env,
    run_once,
    sha256_bytes,
)


def main(argv: Sequence[str] | None = None) -> int:
    try:
        return check_determinism(argv)
    except DeterminismContractError as exc:
        print(f"objc3c-end-to-end-determinism: error: {exc}", file=sys.stderr)
        return 1


__all__ = [
    "DEFAULT_REPLAY_ROOT",
    "MAX_STDIO_PREVIEW_CHARS",
    "MODE",
    "ROOT",
    "SCRIPT_ROOT",
    "DeterminismContractError",
    "build_parser",
    "canonical_json",
    "check_determinism",
    "collect_artifacts",
    "compare_runs",
    "display_path",
    "expand_command_tokens",
    "main",
    "normalize_command_tokens",
    "parse_key_value",
    "parse_variant_env",
    "run_once",
    "sha256_bytes",
]


if __name__ == "__main__":
    raise SystemExit(main())
