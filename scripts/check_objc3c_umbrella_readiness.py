#!/usr/bin/env python3
"""Validate umbrella capability readiness gates and projection drift."""

from __future__ import annotations

import argparse
import sys
from collections.abc import Sequence

from capability_docs_validator.constants import UMBRELLA_READINESS_DOC
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.rendering import render_support_docs
from capability_docs_validator.umbrella_readiness import validate_umbrella_readiness_doc
from capability_docs_validator.validation import _load_validated_inputs


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="fail on readiness projection drift")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        inputs = _load_validated_inputs()
        rendered_docs = render_support_docs(
            matrix=inputs.matrix,
            rows=inputs.rows,
            evidence_map=inputs.evidence_map,
            manifest=inputs.manifest,
            phase_owner_contracts=inputs.phase_owner_contracts,
            umbrella_readiness=inputs.umbrella_readiness,
        )
        if args.check:
            validate_umbrella_readiness_doc(
                UMBRELLA_READINESS_DOC,
                rendered_docs[UMBRELLA_READINESS_DOC],
            )
    except CapabilityDocsError as exc:
        print(f"umbrella readiness validation error: {exc}", file=sys.stderr)
        return 1
    print("umbrella-readiness: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
