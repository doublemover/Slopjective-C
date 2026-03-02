#!/usr/bin/env python3
"""Fail-closed validator for M226-A022 parser advanced edge compatibility workpack."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-advanced-edge-compat-workpack-contract-a022-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_advanced_edge_compat_workpack_a022_expectations.md",
    "sema_handoff": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M226-A022-DOC-01",
            "Contract ID: `objc3c-parser-advanced-edge-compat-workpack-contract/m226-a022-v1`",
        ),
        (
            "M226-A022-DOC-02",
            "`python scripts/check_m226_a022_parser_advanced_edge_compat_workpack_contract.py`",
        ),
        (
            "M226-A022-DOC-03",
            "`python -m pytest tests/tooling/test_check_m226_a022_parser_advanced_edge_compat_workpack_contract.py -q`",
        ),
        (
            "M226-A022-DOC-04",
            "Legacy normalization (`Objc3SemaCompatibilityMode::Legacy`) repairs the",
        ),
    ),
    "sema_handoff": (
        ("M226-A022-SEM-01", "snapshot.protocol_property_decl_count > protocol_property_count"),
        (
            "M226-A022-SEM-02",
            "(snapshot.protocol_method_decl_count > protocol_method_count) ||",
        ),
        ("M226-A022-SEM-03", "snapshot.interface_property_decl_count > interface_property_count"),
        ("M226-A022-SEM-04", "snapshot.interface_method_decl_count > interface_method_count"),
        (
            "M226-A022-SEM-05",
            "snapshot.implementation_property_decl_count > implementation_property_count",
        ),
        (
            "M226-A022-SEM-06",
            "snapshot.implementation_method_decl_count > implementation_method_count",
        ),
        (
            "M226-A022-SEM-07",
            "if (normalized_snapshot.protocol_property_decl_count > protocol_property_count) {",
        ),
        (
            "M226-A022-SEM-08",
            "if (normalized_snapshot.protocol_method_decl_count > protocol_method_count) {",
        ),
        (
            "M226-A022-SEM-09",
            "if (normalized_snapshot.interface_property_decl_count > interface_property_count) {",
        ),
        (
            "M226-A022-SEM-10",
            "if (normalized_snapshot.interface_method_decl_count > interface_method_count) {",
        ),
        (
            "M226-A022-SEM-11",
            "if (normalized_snapshot.implementation_property_decl_count >",
        ),
        (
            "M226-A022-SEM-12",
            "if (normalized_snapshot.implementation_method_decl_count >",
        ),
        ("M226-A022-SEM-13", "if (compatibility_mode != Objc3SemaCompatibilityMode::Legacy) {"),
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
        default=Path("tmp/reports/m226/M226-A022/parser_advanced_edge_compat_workpack_summary.json"),
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
