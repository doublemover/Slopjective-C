#!/usr/bin/env python3
"""Generate or check docs/support markdown projections."""

from __future__ import annotations

import argparse
import difflib
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import repo_rel

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.rendering import render_support_docs
from capability_docs_validator.validation import _load_validated_inputs


def _write_docs(rendered_docs: dict[Path, str]) -> None:
    for path, text in rendered_docs.items():
        path.write_text(text, encoding="utf-8")
        print(f"wrote {repo_rel(path)}")


def _check_docs(rendered_docs: dict[Path, str]) -> int:
    drifted = False
    for path, expected in rendered_docs.items():
        actual = path.read_text(encoding="utf-8") if path.is_file() else ""
        if actual == expected:
            continue
        drifted = True
        print(f"support docs drift: {repo_rel(path)}", file=sys.stderr)
        diff = difflib.unified_diff(
            actual.splitlines(),
            expected.splitlines(),
            fromfile=f"{repo_rel(path)} (current)",
            tofile=f"{repo_rel(path)} (generated)",
            lineterm="",
        )
        for line in diff:
            print(line, file=sys.stderr)
    if drifted:
        return 1
    print("capability-support-docs: PASS")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="fail on generated docs drift")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        inputs = _load_validated_inputs()
    except CapabilityDocsError as exc:
        print(f"capability support docs input error: {exc}", file=sys.stderr)
        return 1
    rendered_docs = render_support_docs(
        matrix=inputs.matrix,
        rows=inputs.rows,
        evidence_map=inputs.evidence_map,
        manifest=inputs.manifest,
        phase_owner_contracts=inputs.phase_owner_contracts,
    )
    if args.check:
        return _check_docs(rendered_docs)
    _write_docs(rendered_docs)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
