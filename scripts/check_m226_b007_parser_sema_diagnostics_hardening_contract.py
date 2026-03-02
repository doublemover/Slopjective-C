#!/usr/bin/env python3
"""Fail-closed validator for M226-B007 parser-sema diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-sema-diagnostics-hardening-contract-b007-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_pass_manager": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_sema_diagnostics_hardening_b007_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_manager": (
        ("M226-B007-SEM-01", "bool diagnostics_accounting_consistent = true;"),
        ("M226-B007-SEM-02", "bool diagnostics_bus_publish_consistent = true;"),
        ("M226-B007-SEM-03", "std::size_t expected_diagnostics_size = 0u;"),
        ("M226-B007-SEM-04", "const std::size_t diagnostics_bus_count_before_publish = input.diagnostics_bus.Count();"),
        ("M226-B007-SEM-05", "diagnostics_accounting_consistent ="),
        ("M226-B007-SEM-06", "result.diagnostics.size() == expected_diagnostics_size;"),
        (
            "M226-B007-SEM-07",
            "const bool pass_bus_publish_consistent = input.diagnostics_bus.diagnostics == nullptr ||",
        ),
        ("M226-B007-SEM-08", "diagnostics_bus_count_after_publish =="),
        ("M226-B007-SEM-09", "const std::size_t diagnostics_emitted_total = std::accumulate("),
        (
            "M226-B007-SEM-10",
            "const bool diagnostics_emission_totals_consistent = diagnostics_emitted_total == result.diagnostics.size();",
        ),
        ("M226-B007-SEM-11", "const bool diagnostics_after_pass_monotonic ="),
        ("M226-B007-SEM-12", "result.deterministic_semantic_diagnostics ="),
        (
            "M226-B007-SEM-13",
            "diagnostics_bus_publish_consistent && diagnostics_emission_totals_consistent &&",
        ),
        (
            "M226-B007-SEM-14",
            "if (!result.deterministic_semantic_diagnostics) {\n    return result;\n  }",
        ),
    ),
    "contract_doc": (
        (
            "M226-B007-DOC-01",
            "Contract ID: `objc3c-parser-sema-diagnostics-hardening-contract/m226-b007-v1`",
        ),
        ("M226-B007-DOC-02", "`result.diagnostics.size() == expected_diagnostics_size`"),
        ("M226-B007-DOC-03", "`sum(diagnostics_emitted_by_pass) == diagnostics.size()`"),
        (
            "M226-B007-DOC-04",
            "`diagnostics_bus_count_after_publish == diagnostics_bus_count_before_publish + pass_diagnostics.size()`",
        ),
        (
            "M226-B007-DOC-05",
            "`python scripts/check_m226_b007_parser_sema_diagnostics_hardening_contract.py`",
        ),
        (
            "M226-B007-DOC-06",
            "`python -m pytest tests/tooling/test_check_m226_b007_parser_sema_diagnostics_hardening_contract.py -q`",
        ),
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
        default=Path("tmp/reports/m226/M226-B007/parser_sema_diagnostics_hardening_summary.json"),
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

