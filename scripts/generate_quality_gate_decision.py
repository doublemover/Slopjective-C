#!/usr/bin/env python3
"""Generate deterministic EV-06..EV-08 quality-gate decision artifacts."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Sequence

try:
    from quality_gate_decision_contracts import (
        ContractDriftError,
        ContractHardFailError,
        build_quality_gate_decision_artifacts,
        quality_gate_status_line,
    )
except ModuleNotFoundError:
    from scripts.quality_gate_decision_contracts import (
        ContractDriftError,
        ContractHardFailError,
        build_quality_gate_decision_artifacts,
        quality_gate_status_line,
    )

DEFAULT_MD = Path("reports/releases/v011_quality_gate_decision.md")
DEFAULT_STATUS = Path("reports/releases/v011_quality_gate_decision.status.json")
DEFAULT_GENERATED_AT = "2026-02-23T22:00:00Z"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_quality_gate_decision.py",
        description="Generate deterministic quality gate markdown and status JSON.",
    )
    parser.add_argument("--output-md", type=Path, default=DEFAULT_MD)
    parser.add_argument("--output-status", type=Path, default=DEFAULT_STATUS)
    parser.add_argument("--generated-at", default=DEFAULT_GENERATED_AT)
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    try:
        artifacts = build_quality_gate_decision_artifacts(args.generated_at)
        args.output_md.parent.mkdir(parents=True, exist_ok=True)
        args.output_status.parent.mkdir(parents=True, exist_ok=True)
        args.output_md.write_text(
            artifacts.markdown + "\n",
            encoding="utf-8",
            newline="\n",
        )
        args.output_status.write_text(
            json.dumps(artifacts.status, indent=2) + "\n",
            encoding="utf-8",
            newline="\n",
        )
    except ContractDriftError as exc:
        print(f"quality-gate-generator: DRIFT ({exc})", file=sys.stderr)
        return 1
    except (ContractHardFailError, OSError) as exc:
        print(f"quality-gate-generator: HARD-FAIL ({exc})", file=sys.stderr)
        return 2

    print(quality_gate_status_line(artifacts))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
