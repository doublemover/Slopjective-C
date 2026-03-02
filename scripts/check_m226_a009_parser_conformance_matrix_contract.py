#!/usr/bin/env python3
"""Fail-closed validator for M226-A009 parser conformance matrix."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-conformance-matrix-contract-a009-v1"

ARTIFACTS: dict[str, Path] = {
    "parser_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_conformance_matrix_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_source": (
        ("M226-A009-PARSE-01", "unsupported pointer type '"),
        ("M226-A009-PARSE-02", "trailing ',' in C-style compatibility parameter list is not allowed"),
        (
            "M226-A009-PARSE-03",
            "expected parameter declaration after ',' in C-style compatibility parameter list, found ",
        ),
        ("M226-A009-PARSE-04", "expected function identifier, found "),
    ),
    "contract_doc": (
        ("M226-A009-DOC-01", "Contract ID: `objc3c-parser-conformance-matrix-contract/m226-a009-v1`"),
        ("M226-A009-DOC-02", "| `A009-C001` | `void *opaque` parameter | Accept | n/a |"),
        ("M226-A009-DOC-03", "| `A009-C002` | `i32 *bad` parameter | Reject | `O3P114` |"),
        ("M226-A009-DOC-04", "| `A009-C003` | `bool *bad` parameter | Reject | `O3P114` |"),
        ("M226-A009-DOC-05", "| `A009-C004` | Trailing comma before `)` | Reject | `O3P104` |"),
        (
            "M226-A009-DOC-06",
            "| `A009-C005` | Non-declaration token after comma | Reject | `O3P100` |",
        ),
        ("M226-A009-DOC-07", "| `A009-C006` | Missing function identifier | Reject | `O3P101` |"),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m226/M226-A009/parser_conformance_matrix_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
